/*
 *      Simple thread class
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <spdlog/spdlog.h>

#include <thread>
#include <atomic>
#include <string>
#include <exception>
#include <functional>

namespace SCI::BAT
{
    class Thread
    {
        public:
            enum class ExecutionResult
            {
                Undefined,
                StoppedNormaly,
                StoppedException,
            };

        public:
            Thread() = default;
            Thread(const Thread&) = delete;
            Thread(Thread&&) noexcept = delete;
            virtual ~Thread();
            
            Thread& operator=(const Thread&) = delete;
            Thread& operator=(Thread&&) noexcept = delete;

            void Start();
            void Stop();
            void Wait(bool requestStop = false);

            inline bool IsFinished() const noexcept
            {
                return m_finished.test(std::memory_order::relaxed);
            }
            inline ExecutionResult GetExecutionResult() const noexcept
            {
                return m_result;
            }
            inline const std::string_view GetExceptionText() const
            {
                return m_exceptionText;
            }
            inline int GetReturnCode() const noexcept
            {
                return m_threadReturnCode;
            }

        protected:
            virtual int ThreadMain() = 0;
            virtual void OnStop() {};

            inline bool StopRequested()
            {
                return m_stopToken ? m_stopToken->stop_requested() : true;
            }

        private:
            void RootThreadMain(std::stop_token stop);

        private:
            bool m_started = false;
            std::atomic_flag m_finished;
            std::jthread m_threadHandle;

            ExecutionResult m_result = ExecutionResult::Undefined;
            std::string m_exceptionText;
            int m_threadReturnCode = -1;

            std::stop_token* m_stopToken = nullptr;
    };
}
