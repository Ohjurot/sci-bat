 /*!
  * @file SPDLogable.h
  * @brief Base class for all classes that should support logging with SPDLog.
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma
#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>

namespace SCI::Util
{
    /*!
     * @brief Base class that provides logging capability.
     * 
     * This class will allow the getting and setting of a SPDLog logger. It provides a way to have object dependent logging.
    */
    class SPDLogable
    {
        public:
            SPDLogable() = default;
            SPDLogable(const SPDLogable&) = default;
            SPDLogable(SPDLogable&&) noexcept = default;
            
            SPDLogable& operator=(const SPDLogable&) = default;
            SPDLogable& operator=(SPDLogable&&) = default;

            /*!
             * @brief Sets the objects logger to the logger supplied by this call.
             * @param logger Logger to be used on this object.
            */
            inline void SetLogger(const std::shared_ptr<spdlog::logger>& logger)
            {
                m_logger = logger;
            }

            /*!
             * @brief Returns the logger that is currently beeing used by this object.
             * @return Pointer to spdlog logger.
            */
            inline std::shared_ptr<spdlog::logger> GetLogger() const
            {
                return m_logger;
            }

        private:
            std::shared_ptr<spdlog::logger> m_logger = spdlog::default_logger();
    };
}
