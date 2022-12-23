 /*!
  * @file Exception.h
  * @brief Singleton for registering and receiving (keyboard) interrupts.
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
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
    /*!
     * @brief Class that registers and receives system signals. 
     *
     * All termination related signals are registered an handled in a uniform way.
    */
    class KeyboardInterrupt : public SPDLogable
    {
        // Class
        public:
            /*!
             * @brief Performs the registration of event signals.
            */
            void Register();

            /*!
             * @brief Checks if an interrupt was received.
             * @return will return true as soon as the first interrupt (signal) was received by this class.
            */
            inline bool InterrupRecived() const noexcept
            {
                return m_interruptRecived || !m_registered;
            }

            /*!
             * @brief Registers a callback that should be called if an interrupt occurred.
             * @tparam T Userdata Type
             * @tparam F Callback Type
             * @param callback Pointer to callback function / lambda / etc. This function will be called if an interrupt occures.
             * @param userdata Pointer to userdata as T*.
            */
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

            /*!
             * @brief Retrives the singelton instance.
             * @return Reference to one and only instance of this class.
            */
            static inline KeyboardInterrupt& Get()
            {
                return s_instance;
            }

        private:
            KeyboardInterrupt() = default;
            static KeyboardInterrupt s_instance;
    };
}
