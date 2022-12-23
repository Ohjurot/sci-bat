 /*!
  * @file LockGuard.h
  * @brief Automatically (scope based) guarding of an lock.
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma once

#include <SCIUtil/Concurrent/ILock.h>

#include <type_traits>

namespace SCI::Util
{
    /*!
     * @brief Automatic lock controller class.
     * 
     * This class will acquire a lock during its constructor. The lock is released on demand or on object destruction.
    */
    class LockGuard
    {
        public:
            /*!
             * @brief Creates a new LockGuard based on a lock. Will block (call pause function) until lock was acquired.
             * @tparam PF Type of pause function.  
             * @tparam ...Args Types of pause function arguments.
             * @param lock Reference to lock that shall be guarded.
             * @param f Pause function to be used.
             * @param ...args Arguments for pause function.
            */
            template<typename PF = void(*)(void), typename... Args, typename = std::enable_if_t<std::is_invocable_v<PF, Args...>>>
            LockGuard(ILock& lock, PF f = &std::this_thread::yield, Args... args) :
                m_lock(lock)
            {
                m_lock.Aquire(f, std::forward<Args>(args)...);
            }

            LockGuard(const LockGuard&) = delete;
            LockGuard(LockGuard&& other) noexcept = delete;

            /*!
             * @brief Destructor will release a pending lock.
            */
            ~LockGuard()
            {
                Release();
            }

            LockGuard& operator=(const LockGuard&) = delete;
            LockGuard& operator=(LockGuard&&) noexcept = delete;

            /*!
             * @brief This function will release the prior locked lock.
            */
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
