/*
 *      Manages a set of threads
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Threading/Thread.h>

#include <vector>

namespace SCI::BAT
{
    class ThreadManager
    {
        private:
            enum class Status
            {
                Prepared,
                Executing,
                Stoped,
            };

        public:
            ThreadManager();
            ThreadManager(const ThreadManager&) = delete;
            ThreadManager(ThreadManager&& other) noexcept = default;
            ~ThreadManager();
            
            ThreadManager& operator=(const ThreadManager&) = delete;
            ThreadManager& operator=(ThreadManager&& other) noexcept = default;

            void Register(Thread& thread);

            void Start();
            void Stop();
            void Wait();

            // Update step
            void Update();

            size_t IsRunning();

            inline size_t operator()()
            {
                if (IsRunning())
                {
                    Update();
                    return true;
                }
                return false;
            }

            inline ThreadManager& operator<<(Thread& thread)
            {
                Register(thread);
                return *this;
            }

        private:
            Status m_status = Status::Prepared;
            std::vector<Thread*> m_threads;
    };
}

