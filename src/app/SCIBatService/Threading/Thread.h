 /*!
  * @file Thread.h
  * @brief Simple thread class.
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
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
    /*!
     * @brief Simple but controllable thread.
     * 
     * This class in meant to used as a base class.
    */
    class Thread
    {
        public:
            /*!
             * @brief Result of a thread execution.
            */
            enum class ExecutionResult
            {
                /*! Thread is not started or still running. */
                Undefined,
                /*! Thread stopped normally. */
                StoppedNormaly,
                /*! Thread stopped due to an exception */
                StoppedException,
            };

        public:
            Thread() = default;
            Thread(const Thread&) = delete;
            Thread(Thread&&) noexcept = delete;
            virtual ~Thread();
            
            Thread& operator=(const Thread&) = delete;
            Thread& operator=(Thread&&) noexcept = delete;

            /*!
             * @brief Start the thread.
            */
            void Start();
            /*!
             * @brief Request the thread to stop. The thread needs to respond to the request.
            */
            void Stop();
            /*!
             * @brief Waits for the thread to be finished.
             * @param requestStop If true a stop request will be sent prior to waiting.
            */
            void Wait(bool requestStop = false);

            /*!
             * @brief Requests the thread to reload its config.
            */
            inline void ConfigReload()
            {
                m_confcReq.test_and_set(std::memory_order::acquire);
            }

            /*!
             * @brief Checks if the thread has finished executing.
             * @return True is thread is finished.
            */
            inline bool IsFinished() const noexcept
            {
                return m_started ? m_finished.test(std::memory_order::relaxed) : true;
            }
            /*!
             * @brief Retrieves the execution result of the thread.
             * @return Only valid when thread is finished. Else Undefined.
            */
            inline ExecutionResult GetExecutionResult() const noexcept
            {
                return m_result;
            }
            /*!
             * @brief Retrieves the exception text of the thread.
             * @return Exception text of the thread as string. Only valid when thread is finished and GetExecutionResult() returned StoppedException.
            */
            inline const std::string_view GetExceptionText() const
            {
                return m_exceptionText;
            }
            /*!
             * @brief Retrieves the return code of the thread.
             * @return This is only valid when the thread finished execution.
            */
            inline int GetReturnCode() const noexcept
            {
                return m_threadReturnCode;
            }

            /*!
             * @brief Checks if this thread should reload its configuration.
             * @return True if the thread should reload configurations.
            */
            inline bool ConfigReloadRequested()
            {
                return m_confcReq.test(std::memory_order::acquire);
            }
            /*!
             * @brief Checks if this thread raises a global config reload.
             * @return True if this thread requested a global config reload.
            */
            inline bool ConfigReloadInitiated()
            {
                bool init = m_confcInit.test(std::memory_order::acquire);
                m_confcInit.clear(std::memory_order::release);
                return init;
            }

        protected:
            virtual int ThreadMain() = 0;
            virtual void OnStop() {};

            /*!
             * @brief Raises a request for a global config change.
            */
            inline void RaisConfigChange()
            {
                m_confcInit.test_and_set(std::memory_order::acquire);
            }
            /*!
             * @brief Marks that this thread has finished reloading its config.
            */
            void DoneConfigChange()
            {
                m_confcReq.clear(std::memory_order::release);
            }

            /*!
             * @brief Checks if a stop request for this thread was set.
             * @return True if stop was requested.
            */
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

            std::atomic_flag m_confcInit;
            std::atomic_flag m_confcReq;
    };
}
