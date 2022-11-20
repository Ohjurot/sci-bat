/*
 *      Will automatically acquire and release a lock
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <SCIUtil/Concurrent/ILock.h>

#include <type_traits>

namespace SCI::Util
{
    class LockGuard
    {
        public:
            LockGuard(ILock& lock) : 
                m_lock(lock)
            {
                m_lock.Aquire();
            }

            template<typename T = void(*)(void), typename = std::enable_if_t<std::is_invocable_v<T>>>
            LockGuard(ILock& lock, T f) :
                m_lock(lock)
            {
                m_lock.Aquire(f);
            }

            template<typename Arg, typename T = void(*)(Arg), typename = std::enable_if_t<std::is_invocable_v<T, Arg>>>
            LockGuard(ILock& lock, T f, Arg arg) :
                m_lock(lock)
            {
                m_lock.Aquire(f, arg);
            }

            template<typename... Args, typename T = void(*)(Args...), typename = std::enable_if_t<std::is_invocable_v<T, Args...>>>
            LockGuard(ILock& lock, T f, Args... args) :
                m_lock(lock)
            {
                m_lock.Aquire(f, args...);
            }

            LockGuard(const LockGuard&) = delete;
            LockGuard(LockGuard&& other) noexcept = delete;

            ~LockGuard()
            {
                Release();
            }

            LockGuard& operator=(const LockGuard&) = delete;
            LockGuard& operator=(LockGuard&&) noexcept = delete;

            void Release()
            {
                if (m_aquired)
                {
                    m_lock.Release();
                    m_aquired = false;
                }
            }

        private:
            ILock& m_lock;
            bool m_aquired = true;
    };
}
