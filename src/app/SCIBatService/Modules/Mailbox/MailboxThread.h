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
                SetLogger(logger);
                LoadConfig();
            }

            int ThreadMain() override;
            void OnStop() override;

            bool Publish(const std::filesystem::path& subTopic, const std::string& text);
            bool GetMQTTMessage(const std::filesystem::path& subTopic, std::string& msgOut);

            void on_message(const struct mosquitto_message*) override;

        private:
            void LoadConfig();

            bool MQTTConnect();
            void MQTTDisconnect();

        private:
            Util::SpinLock m_lock;
            Util::SpinLock m_mosqLock;

            std::unordered_map<std::string, std::string> m_mqttInbox;

            std::string m_brokerAddress = "localhost";
            int m_brokerPort = 1883;
            std::filesystem::path m_baseTopic = "sci-bat";
            bool m_isConnected = false;
            bool m_hasError = false;
    };
}
