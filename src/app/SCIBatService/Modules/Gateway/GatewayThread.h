#pragma once

#include <Threading/Thread.h>

#include <SCIUtil/SPDLogable.h>

namespace SCI::BAT::Gateway
{
    class GatewayThread : public Thread, public Util::SPDLogable
    {
        public:
            GatewayThread(const std::shared_ptr<spdlog::logger>& gatewayLogger = spdlog::default_logger());

        protected:
            int ThreadMain() override;
            void OnStop() override;

        private:

    };
}
