#include <RETIUtil/KeyboardInterrupt.h>

#include <pugixml.hpp>
#include <spdlog/spdlog.h>
#include <mosquittopp.h>

#include <iostream>
#include <string>
#include <chrono>
#include <thread>

class MyMQTTClient : public mosqpp::mosquittopp
{
public:
    MyMQTTClient() : mosqpp::mosquittopp("RETI-TestClient") {}


    void on_publish(int mid) override
    {
        spdlog::info("Message {} Published", mid);
    }


    void on_subscribe(int mid, int rqos, const int* gqos) override
    {
        spdlog::info("Message {} subscribed with a final qos of {} (Requested {})!", mid, *gqos, rqos);
    }


    void on_unsubscribe(int mid) override
    {
        spdlog::info("Unsubscribed to message {}!", mid);
    }

    void on_error() override
    {
        spdlog::error("An mosquitto error occured!");
    }

    void on_connect(int rc) override
    {
        spdlog::info("Connected to MQTT broker with rc = {}", rc);
    }

    void on_message(const mosquitto_message* msg) override
    {
        spdlog::info(R"(Retrive message on topic "{}" with the value "{}")", msg->topic, (const char*)msg->payload, msg->mid);
    }
};

int main()
{
    using namespace std::chrono_literals;

    // Load settings
    pugi::xml_document settings;
    auto settings_load_result = settings.load_file("./settings.xml");
    if (!settings_load_result)
    {
        spdlog::error(R"(Failed to open "./settings.xml")");
        return -1;
    }

    // Seek root configuration node
    auto settings_node = settings.child("Settings");
    if (!settings_node)
    {
        spdlog::error(R"(Failed to retrieve "Settings" xml node.)");
        return -1;
    }
    auto mqtt_node = settings_node.child("MQTT");
    if (!mqtt_node)
    {
        spdlog::error(R"(Failed to retrieve "Settings/MQTT" xml node.)");
        return -1;
    }

    // Get all attributes
    auto qos_att = mqtt_node.attribute("qos");
    auto retain_att = mqtt_node.attribute("retain");
    auto topic_prefix_att = mqtt_node.attribute("topic_prefix");

    // Extract settings
    uint32_t setting_qos = qos_att.as_uint(0);
    bool setting_retain = retain_att.as_bool(true);
    std::string setting_topic_prefix = topic_prefix_att.as_string("");

    // Validate settings
    if (setting_qos > 2)
    {
        spdlog::error(R"(A qos value of {} is not valid! Valid qos values are 0, 1 and 2!)", setting_qos);
        return -1;
    }
    if (setting_topic_prefix.empty())
    {
        spdlog::error(R"(An empty topic prefix is not allowed. We need to separate this from all the junk coming into the "test.mosquitto.org" server.)");
        return -1;
    }

    // Init mqtt
    if (mosqpp::lib_init() != MOSQ_ERR_SUCCESS)
    {
        spdlog::error("Failed to init mosquitto lib!");
        return -1;
    }

    // Test
    MyMQTTClient mq;
    int res;
    if ((res = mq.connect("test.mosquitto.org", 1883)) != MOSQ_ERR_SUCCESS) // Unauthenticated and unencrypted
    {
        spdlog::error(R"(Unable to connect to "test.mosquitto.org:1883")");
        return -1;
    }

    // Subscribe
    std::stringstream ss;
    ss << setting_topic_prefix << "/" << "example";
    auto stopic = ss.str();
    int sub_mid = 0;
    mq.subscribe(&sub_mid, stopic.c_str(), 0);

    // KB Interrupt
    auto& kbInterupt = RETI::Util::KeyboardInterrupt::Get();
    kbInterupt.Register();

    // Big loop
    int counter = 0;
    while (!kbInterupt.InterupRecived())
    {
        // Send a message
        if (counter++ == 50)
        {
            counter = 0;
            ss.str("");
            ss << setting_topic_prefix << "/" << "example";
            const char* payload = "Hello MQTT!";
            int pub_mid = 0;
            mq.publish(&pub_mid, ss.str().c_str(), strlen(payload) + 1, payload);
        }

        // Loop
        auto rc = mq.loop(50);
        if (rc)
        {
            mq.reconnect();
        }
    }

    // Unsub
    int usub_mid = 0;
    mq.unsubscribe(&usub_mid, stopic.c_str());
    mq.disconnect();

    mosqpp::lib_cleanup();
}
