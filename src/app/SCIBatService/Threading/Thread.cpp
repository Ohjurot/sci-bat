#include "Thread.h"

SCI::BAT::Thread::~Thread()
{
    Stop();
    Wait();
    if (m_threadHandle.joinable())
    {
        m_threadHandle.join();
    }
}

void SCI::BAT::Thread::Start()
{
    if (!m_started)
    {
        m_threadHandle = std::move(std::jthread(std::bind_front(&Thread::RootThreadMain, this)));
        m_started = true;
    }
}

void SCI::BAT::Thread::Stop()
{
    if (m_started && m_threadHandle.joinable())
    {
        OnStop();
        m_threadHandle.request_stop();
    }
}

void SCI::BAT::Thread::Wait(bool requestStop /*= false*/)
{
    // Stop request
    if (requestStop)
    {
        Stop();
    }

    // wait
    while (!IsFinished())
    {
        std::this_thread::yield();
    }
}

void SCI::BAT::Thread::RootThreadMain(std::stop_token stop)
{   
    try
    {
        m_stopToken = &stop;
        m_tid = std::this_thread::get_id();
        m_threadReturnCode = ThreadMain();
        m_result = ExecutionResult::StoppedNormaly;
    }
    catch (std::exception& ex)
    {
        spdlog::critical("Exception in thread occurred (thread is terminating!): {}", ex.what());
        m_exceptionText = ex.what();
        m_result = ExecutionResult::StoppedException;
    }
    catch (...)
    {
        spdlog::critical("Unknown exception in thread occurred! Thread is terminating!");
        m_exceptionText = "Unknown exception";
        m_result = ExecutionResult::StoppedException;
    }

    // REACHED STOP
    m_finished.test_and_set(std::memory_order::acquire);
}
