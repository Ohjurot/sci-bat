#pragma once

#include <atomic>
#include <thread>
#include <functional>

namespace SCI::Util
{
    // (S)hared(R)ead(E)xclusive(W)rite
    // Multiple reads a time allowed
    // Only onw write access allowed (will block reads)
    class SREWLock
    {
        public:
            SREWLock() = default;
            SREWLock(const SREWLock&) = delete;
            SREWLock(SREWLock&&) noexcept = default;

            SREWLock& operator=(const SREWLock&) = delete;
            SREWLock& operator=(SREWLock&&) noexcept = default;

            void AquireWrite(const std::function<void(void)>& yield = &std::this_thread::yield);
            bool TryAquireRead();
            inline void AquireRead(const std::function<void(void)>& yield = &std::this_thread::yield)
            {
                while (!TryAquireRead()) yield();
            }
            void ReleaseWrite();
            void ReleaseRead();

        private:
            std::atomic_flag m_writeLock;
            std::atomic<size_t> m_readCounter = 0;

        public:
            class WriteGuard
            {
                public:
                    WriteGuard() = delete;
                    WriteGuard(const WriteGuard&) = delete;
                    WriteGuard(WriteGuard&&) noexcept = delete;

                    WriteGuard& operator=(const WriteGuard&) = delete;
                    WriteGuard& operator=(WriteGuard&&) noexcept = delete;

                    WriteGuard(SREWLock& lock) : m_lock(lock)
                    {
                        m_lock.AquireWrite();
                    }

                    ~WriteGuard()
                    {
                        Release();
                    }

                    void Release()
                    {
                        if (m_aquired)
                        {
                            m_lock.ReleaseWrite();
                            m_aquired = false;
                        }
                    }

                private:
                    SREWLock& m_lock;
                    bool m_aquired = true;
            };
            class ReadGuard
            {
                public:
                    ReadGuard() = delete;
                    ReadGuard(const ReadGuard&) = delete;
                    ReadGuard(ReadGuard&&) noexcept = delete;

                    ReadGuard& operator=(const ReadGuard&) = delete;
                    ReadGuard& operator=(ReadGuard&&) noexcept = delete;

                    ReadGuard(SREWLock& lock) : m_lock(lock)
                    {
                        m_lock.AquireRead();
                    }

                    ~ReadGuard()
                    {
                        Release();
                    }

                    void Release()
                    {
                        if (m_aquired)
                        {
                            m_lock.ReleaseRead();
                            m_aquired = false;
                        }
                    }

                private:
                    SREWLock& m_lock;
                    bool m_aquired = true;
            };
    };
}
