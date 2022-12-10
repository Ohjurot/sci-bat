#pragma once

#include <Threading/Thread.h>
#include <Config/AuthenticatedConfig.h>
#include <Modules/Gateway/SMAData.h>
#include <Modules/Mailbox/MailboxThread.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <SCIUtil/SPDLogable.h>
#include <SCIUtil/Exception.h>
#include <SCIUtil/Concurrent/SpinLock.h>
#include <SCIUtil/Concurrent/LockGuard.h>
#include <ModbusMaster/Master.h>

namespace SCI::BAT::Gateway
{
    class GatewayThread : public Thread, public Util::SPDLogable
    {
        public:
            GatewayThread(Mailbox::MailboxThread& mailbox, const std::shared_ptr<spdlog::logger>& gatewayLogger = spdlog::default_logger());

            static inline SMAInData GetInputData()
            {
                SCI_ASSERT(s_gateway, "Gateway not initialized");
                Util::LockGuard janitor(s_gateway->m_dataLock);
                return s_gateway->m_smaInputData;
            }
            static inline SMAOutData GetOuputData()
            {
                SCI_ASSERT(s_gateway, "Gateway not initialized");
                Util::LockGuard janitor(s_gateway->m_dataLock);
                return s_gateway->m_smaOutputData;
            }

        protected:
            int ThreadMain() override;
            void OnStop() override;

            void PublishMQTTInfo(const SMAInData& id, const SMAOutData& od);

            static void SMAReadInputData(Modbus::Master& modbus, SMAInData& smaIn);
            static void SMAWriteOutputData(Modbus::Master& modbus, SMAOutData& smaOut);

        private:
            void LoadConfig();

        private:
            static GatewayThread* s_gateway;

            Util::SpinLock m_dataLock;
            SMAInData m_smaInputData;
            SMAOutData m_smaOutputData;

            std::string m_smaIp = "0.0.0.0";
            int m_smaSlaveNode = 3;
            int m_smaPort = 502;
            int m_refRateInMs = 3000;

            Modbus::Master m_modbus;
            Mailbox::MailboxThread& m_mailbox;
    };
}
