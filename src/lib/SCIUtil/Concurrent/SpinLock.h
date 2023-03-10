 /*!
  * @file SpinLock.h
  * @brief Simple and fast spin lock (Atomic).
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma once

#include <SCIUtil/Concurrent/ILock.h>

#include <atomic>

namespace SCI::Util
{
    /*!
     * @brief Implements a fast (atomic) spin lock.
    */
    class SpinLock : public ILock
    {
        public:
            bool TryAquire() override
            {
                return !m_flag.test_and_set(std::memory_order::acquire);
            }

            void Release() override
            {
                m_flag.clear(std::memory_order::release);
            }

        private:
            std::atomic_flag m_flag;
    };
}
