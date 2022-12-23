 /*!
  * @file ILock.h
  * @brief Interface for concurrent locks.
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma once

#include <type_traits>
#include <thread>

namespace SCI::Util
{
    /*!
     * @brief Interface defining the behavior of concurrent locks.
    */
    class ILock
    {
        public:
            virtual ~ILock() = default;
            
            /*!
             * @brief Tries to acquire a lock.
             * @return True if lock was acquired. False if acquisition fails.
            */
            virtual bool TryAquire() = 0;

            /*!
             * @brief Releases a lock.
            */
            virtual void Release() = 0;

            /*!
             * @brief Aquires th lock. Will call the pause function with supplied arguments as long as TryAquire() failes.
             * @tparam PF Type of pause function.
             * @tparam ...Args Type of pause function arguments.
             * @param f Pause function object / pointer.
             * @param ...args Arguments for the pause function.
            */
            template<typename PF = void(*)(void), typename... Args, typename = std::enable_if_t<std::is_invocable_v<PF, Args...>>>
            inline void Aquire(PF f = &std::this_thread::yield, Args... args)
            {
                while (!TryAquire())
                    f(std::forward<Args>(args)...);
            }
    };
}
