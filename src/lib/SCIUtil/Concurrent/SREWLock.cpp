#include "SREWLock.h"

void SCI::Util::SREWLock::AquireWrite(const std::function<void(void)>& yield)
{
    // 1: Check if current write access is possible and lock it
    while (m_writeLock.test_and_set(std::memory_order::acquire))
    {
        yield();
    }

    // 2: Wait until read handler become zero
    while (m_readCounter.load(std::memory_order::acquire))
    {
        yield();
    }

    // Read to role
}

void SCI::Util::SREWLock::ReleaseWrite()
{
    // 1: Simple releasing
    m_writeLock.clear(std::memory_order::release);
}

void SCI::Util::SREWLock::ReleaseRead()
{
    // 1: Simple decrement
    m_readCounter--;
}

bool SCI::Util::SREWLock::TryAquireRead()
{
    // 1: Requiring a valid acquire lock
    if (!m_writeLock.test(std::memory_order::acquire))
    {
        // --- HERE IS THE POTENTIAL RACE WHERE WRITE LOCK GETS AQUIRED ---

        // 2: Increment read counter
        m_readCounter++;

        // 3: Fix race condition
        if (m_writeLock.test(std::memory_order::memory_order_seq_cst))
        {
            // Race occurred release reference and fail
            m_readCounter--;
            return false;
        }
        
        // Passed
        return true;
    }

    return false;
}
