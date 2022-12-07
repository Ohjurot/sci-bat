#pragma once

#include <Threading/Thread.h>
#include <Modules/Gateway/SMAData.h>

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
            GatewayThread(const std::shared_ptr<spdlog::logger>& gatewayLogger = spdlog::default_logger());

        protected:
            int ThreadMain() override;
            void OnStop() override;

            static void SMAReadInputData(Modbus::Master& modbus, SMAInData& smaIn);
            static void SMAWriteOutputData(Modbus::Master& modbus, SMAOutData& smaOut);

        private:
            Util::SpinLock m_dataLock;
            SMAInData m_smaInputData;
            SMAOutData m_smaOutputData;

            Modbus::Master m_modbus;

    };
}
