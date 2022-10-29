/*
 *      Singleton for registering and receiving (keyboard) interrupts 
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <SCIUtil/SPDLogable.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>

namespace SCI::Util
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
            KeyboardInterrupt(const KeyboardInterrupt&) = delete;
            KeyboardInterrupt& operator=(const KeyboardInterrupt&) = delete;

            static inline KeyboardInterrupt& Get()
            {
                return s_instance;
            }

        private:
            KeyboardInterrupt() = default;
            static KeyboardInterrupt s_instance;
    };
}
