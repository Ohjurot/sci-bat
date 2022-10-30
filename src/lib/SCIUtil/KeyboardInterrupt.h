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
#include <functional>
#include <type_traits>

#include <iostream>

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
            template<typename T, typename F, typename = std::enable_if_t< std::is_pointer_v<T> && std::is_invocable_v<F, int, T> >>
            inline void SetCallback(F callback, T userdata = nullptr)
            {
                m_callback = (void(*)(int,void*))callback;
                m_callbackData = userdata;
            } 

        private:
            static void SignalHandler(int signal);

        private:
            std::function<void(int,void*)> m_callback;
            void* m_callbackData = nullptr;

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
