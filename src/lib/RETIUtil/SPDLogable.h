#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>

namespace RETI::Util
{
    // Is doing logging with spdlog
    class SPDLogable
    {
        public:
            SPDLogable() = default;
            SPDLogable(const SPDLogable&) = default;
            SPDLogable(SPDLogable&&) noexcept = default;
            
            SPDLogable& operator=(const SPDLogable&) = default;
            SPDLogable& operator=(SPDLogable&&) = default;

            inline void SetLogger(const std::shared_ptr<spdlog::logger>& logger)
            {
                m_logger = logger;
            }

            inline std::shared_ptr<spdlog::logger> GetLogger() const
            {
                return m_logger;
            }

        private:
            std::shared_ptr<spdlog::logger> m_logger = spdlog::default_logger();
    };
}
