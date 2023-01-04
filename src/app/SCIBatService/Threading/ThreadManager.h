 /*!
  * @file ThreadManager.h
  * @brief Manages a set of threads.
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma once

#include <Threading/Thread.h>

#include <vector>

namespace SCI::BAT
{
    /*!
     * @brief Container that manages the execution of multiple threads and their synchronization. 
    */
    class ThreadManager
    {
        private:
            /*!
             * @brief Status of the manger.
            */
            enum class Status
            {
                /*! The ThreadManager is prepared for execution. */
                Prepared,
                /*! The ThreadManager is currently executing one or multiple threads. */
                Executing,
                /*! The ThreadManager has finished executing all threads. */
                Stoped,
            };

        public:
            ThreadManager();
            ThreadManager(const ThreadManager&) = delete;
            ThreadManager(ThreadManager&& other) noexcept = default;
            ~ThreadManager();
            
            ThreadManager& operator=(const ThreadManager&) = delete;
            ThreadManager& operator=(ThreadManager&& other) noexcept = default;

            /*!
             * @brief Registers a thread with this ThreadManager.
             * @param thread Thread to be managed by the ThreadManager.
            */
            void Register(Thread& thread);

            /*!
             * @brief Starts the execution of all managed threads.
            */
            void Start();
            /*!
             * @brief Starts the execution of all managed threads.
            */
            void Stop();
            /*!
             * @brief Waits until all threads finish execution.
            */
            void Wait();

            /*!
             * @brief Performs a single thread synchronization step.
            */
            void Update();

            /*!
             * @brief Checks if one or multiple threads requested a global system stop.
             * @return True if a request was raised.
            */
            bool HasSystemStopRequest() const;

            /*!
             * @brief Checks how many threads are currently running.
             * @return Number of threads running.
            */
            size_t IsRunning();

            /*!
             * @brief Execution operator for incremental synchronization. 
             * 
             * Will call Update() when threads are running.
             * @return Returns true as long as at minimum thread in running.
            */
            inline size_t operator()()
            {
                if (IsRunning())
                {
                    Update();
                    return true;
                }
                return false;
            }

            /*!
             * @brief Shift operator for registering thread the iostream way.
             * @param thread Thread to be registered.
             * @return Reference to this for chaining.
            */
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

