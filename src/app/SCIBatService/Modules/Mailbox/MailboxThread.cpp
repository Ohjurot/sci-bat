#include "MailboxThread.h"

int SCI::BAT::Mailbox::MailboxThread::ThreadMain()
{
    using namespace std::chrono_literals;

    MQTTConnect();

    while (!StopRequested())
    {
        // Loop MQTT
        if (m_isConnected)
        {
            Util::LockGuard janitor(m_mosqLock);
            loop(100);
        }

        Util::LockGuard janitor(m_lock);
        
        // Check for errors
        if (m_hasError)
        {
            GetLogger()->warn("MQTT Mailbox had issue. Trying to reconnect.");
            if (!m_isConnected || reconnect_async() != MOSQ_ERR_SUCCESS)
            {
                MQTTDisconnect();
                MQTTConnect();
            }
            m_hasError = false;
        }
        // Update config
        if (ConfigReloadRequested())
        {
            GetLogger()->info("Config change requested! Reloading config.");
            LoadConfig();
            GetLogger()->info("Config change requested! Restarting MQTT connection.");
            MQTTDisconnect();
            MQTTConnect();
            DoneConfigChange();
        }

        // Try connection 
        if (!m_isConnected)
        {
            MQTTConnect();
        }

        janitor.Release();

        // Give the CPU headroom
        std::this_thread::sleep_for(20ms);
    }

    MQTTDisconnect();
    return 0;
}

void SCI::BAT::Mailbox::MailboxThread::OnStop()
{
    // We don't need to catch the event
}


bool SCI::BAT::Mailbox::MailboxThread::Publish(const std::filesystem::path& subTopic, const std::string& text)
{
    // Well... we will block here too...
    Util::LockGuard janitor(m_mosqLock);

    if (m_isConnected)
    {
        auto topic = m_baseTopic / "status" / subTopic;
        GetLogger()->trace("Sending MQTT message on topic \"{}\": \"{}\".", topic.generic_string(), text);
        auto result = publish(nullptr, topic.generic_string().c_str(), text.length(), text.c_str(), 0, true);
        if (result != MOSQ_ERR_SUCCESS)
        {
            if (reconnect_async() == MOSQ_ERR_SUCCESS)
            {
                // Retry
                GetLogger()->debug("Reconnected to mosquitto broker.");
                result = publish(nullptr, topic.generic_string().c_str(), text.length(), text.c_str(), 0, true);
                if (result == MOSQ_ERR_SUCCESS)
                {
                    GetLogger()->trace("MQTT Message send successfully!");
                    return true;
                }
            }

            GetLogger()->warn("Failed to publish MQTT message on topic \"{}\" error code {}.", topic.generic_string(), result);
            m_hasError = true;
        }
        else
        {
            GetLogger()->trace("MQTT Message send successfully!");
            return true;
        }
    }

    return false;
}

bool SCI::BAT::Mailbox::MailboxThread::MQTTConnect()
{
    if (m_isConnected)
        return true;

    GetLogger()->info("Connecting to \"{}:{}\" MQTT Broker.", m_brokerAddress, m_brokerPort);
    
    // Authentication
    if (!m_brokerUsername.empty())
    {
        GetLogger()->info("Connecting with user \"{}\" using password: ", m_brokerUsername, m_brokerPassword.empty() ? "NO" : "YES");
        username_pw_set(m_brokerUsername.c_str(), m_brokerPassword.empty() ? nullptr : m_brokerPassword.c_str());
    }
    else
    {
        username_pw_set(nullptr, nullptr);
    }

    // Connect
    if (connect_async(m_brokerAddress.c_str(), m_brokerPort) == MOSQ_ERR_SUCCESS)
    {
        GetLogger()->info("Successfully connected to MQTT broker.");
        m_isConnected = true;

        // Subscribe to control topic
        auto subscriptionPattern = m_baseTopic / "control" / "#";
        if (subscribe(nullptr, subscriptionPattern.generic_string().c_str()) == MOSQ_ERR_SUCCESS)
        {
            // 
            GetLogger()->info("Successfully subsribed to {} MQTT topic.", subscriptionPattern.generic_string());
            return true;
        }
        else
        {
            GetLogger()->error("Failed to subscribe to MQTT topic!");
            MQTTDisconnect();
        }
    }

    return false;
}

void SCI::BAT::Mailbox::MailboxThread::MQTTDisconnect()
{
    if (m_isConnected)
    {
        disconnect();
        m_isConnected = false;
        GetLogger()->info("Disconnected from MQTT broker.");
    }
}

void SCI::BAT::Mailbox::MailboxThread::on_message(const struct mosquitto_message* msg)
{
    const auto baseTopic = (m_baseTopic / "control").generic_string();
    GetLogger()->trace("Received MQTT message on topic \"{}\" (Message ID: {}).", msg->topic, msg->mid);

    // Extract message
    std::string msgText;
    msgText.assign((const char*)msg->payload, msg->payloadlen);
    
    // Extract topic
    std::string topic = msg->topic;

    auto cPos = topic.find(baseTopic);
    if (cPos == 0)
    {
        auto subTopic = topic.substr(cPos + baseTopic.length() + 1);
        GetLogger()->trace("Decoded MQTT message for topic \"{}\": \"{}\".", subTopic, msgText);

        // Store message
        Util::LockGuard janitor(m_lock);
        m_mqttInbox[subTopic] = msgText;
    }
    else
    {
        GetLogger()->warn("Invalid MQTT Message on topic \"{}\". Topic format not expected.", topic);
    }
}

bool SCI::BAT::Mailbox::MailboxThread::GetMQTTMessage(const std::filesystem::path& subTopic, std::string& msgOut)
{
    Util::LockGuard janitor(m_lock);
    auto itFind = m_mqttInbox.find(subTopic.generic_string());
    if (itFind != m_mqttInbox.end())
    {
        msgOut = itFind->second;
        m_mqttInbox.erase(itFind);
        return true;
    }

    return false;
}

void SCI::BAT::Mailbox::MailboxThread::LoadConfig()
{
    // Assert config node existence
    GetLogger()->debug("Asserting default config.");
    Config::AuthenticateConfig::InsertData(
        "mailbox",
        (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Admin, (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Admin, (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::System,
        {
            {
                "broker",
                {
                    { "address", "localhost" },
                    { "username", "" },
                    { "password", "" },
                    { "port", 1883 }
                }
            },
            { "basetopic", "sci-bat" }
        }
        );

    // Now read current config
    nlohmann::json config;
    GetLogger()->debug("Reading config from db.");
    if (Config::AuthenticateConfig::ReadData("mailbox", (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::System, config))
    {
        m_brokerAddress = config["broker"]["address"];
        m_brokerUsername = config["broker"]["username"];
        m_brokerPassword = config["broker"]["password"];
        m_brokerPort = config["broker"]["port"];
        m_baseTopic = config["basetopic"].get<std::string>();
    }
    else
    {
        GetLogger()->error("Failed to read config.");
    }
}
