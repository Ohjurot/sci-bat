#pragma once

#include <Threading/Thread.h>
#include <Config/AuthenticatedConfig.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <SCIUtil/SPDLogable.h>
#include <SCIUtil/Concurrent/SpinLock.h>
#include <SCIUtil/Concurrent/LockGuard.h>

#include <mosquittopp.h>

#include <filesystem>
#include <string>
#include <unordered_map>

namespace SCI::BAT::Mailbox
{
    class MailboxThread : public Thread, private mosqpp::mosquittopp, public Util::SPDLogable
    {
        public:
            inline MailboxThread(const std::shared_ptr<spdlog::logger>& logger = spdlog::default_logger())
            {
                s_mailbox = this;

                SetLogger(logger);
                LoadConfig();
            }

            int ThreadMain() override;
            void OnStop() override;

            bool Publish(const std::filesystem::path& subTopic, const std::string& text);
            bool GetMQTTMessage(const std::filesystem::path& subTopic, std::string& msgOut);

            void on_message(const struct mosquitto_message*) override;

            static inline auto GetStaticTID()
            {
                return s_mailbox->GetTID();
            }
            static inline auto IsStaticFinished()
            {
                return s_mailbox->IsFinished();
            }
            static inline auto GetConnectionString()
            {
                return s_mailbox->m_brokerUsername.empty() ? 
                    fmt::format("tcp://{}:{}", s_mailbox->m_brokerAddress, s_mailbox->m_brokerPort) :
                    fmt::format("tcp://{}@{}:{} (Using password: {})", s_mailbox->m_brokerUsername, s_mailbox->m_brokerAddress, s_mailbox->m_brokerPort, s_mailbox->m_brokerPassword.empty() ? "NO" : "YES");
            }
            static inline auto GetConnected()
            {
                return s_mailbox->m_mqttUpdated;
            }

        private:
            void LoadConfig();

            bool MQTTConnect();
            void MQTTDisconnect();

        private:
            static MailboxThread* s_mailbox;

            Util::SpinLock m_lock;
            Util::SpinLock m_mosqLock;

            bool m_mqttUpdated = false;

            std::unordered_map<std::string, std::string> m_mqttInbox;

            std::string m_brokerAddress = "localhost";
            std::string m_brokerUsername = "";
            std::string m_brokerPassword = "";
            int m_brokerPort = 1883;
            std::filesystem::path m_baseTopic = "sci-bat";
            bool m_isConnected = false;
            bool m_hasError = false;
    };
}
