#pragma once

#include <RETIUtil/SPDLogable.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>

namespace RETI::Util
{
    class KeyboardInterrupt : public SPDLogable
    {
        // Class
        public:
            void Register();

            inline bool InterupRecived() const noexcept
            {
                return m_interruptRecived || !m_registered;
            }

        private:
            static void SignalHandler(int signal);

        private:
            bool m_registered = false;
            bool m_interruptRecived = false;

        // Singleton
        public:
            static inline KeyboardInterrupt& Get()
            {
                return s_instance;
            }

        private:
            KeyboardInterrupt() = default;
            static KeyboardInterrupt s_instance;
    };
}
