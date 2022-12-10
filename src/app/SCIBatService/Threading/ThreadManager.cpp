#include "ThreadManager.h"

SCI::BAT::ThreadManager::ThreadManager()
{

}

SCI::BAT::ThreadManager::~ThreadManager()
{
    Stop();
    Wait();
}

void SCI::BAT::ThreadManager::Stop()
{
    if (m_status == Status::Executing)
    {
        for (auto* thread : m_threads)
        {
            thread->Stop();
        }

        m_status = Status::Stoped;
    }
}

void SCI::BAT::ThreadManager::Start()
{
    if (m_status == Status::Prepared)
    {
        for (auto* thread : m_threads)
        {
            thread->Start();
        }

        m_status = Status::Executing;
    }
}

size_t SCI::BAT::ThreadManager::IsRunning()
{
    size_t runningThreads = 0;
    if (m_status == Status::Executing)
    {
        for (auto* thread : m_threads)
        {
            if (!thread->IsFinished())
            {
                runningThreads++;
            }
        }
    }

    return runningThreads;
}

void SCI::BAT::ThreadManager::Register(Thread& thread)
{
    if (m_status == Status::Prepared)
    {
        m_threads.push_back(&thread);
    }
}

void SCI::BAT::ThreadManager::Wait()
{
    if (m_status == Status::Stoped)
    {
        // Wait for all threads
        for (auto* thread : m_threads)
        {
            thread->Wait();
        }
    }
}

void SCI::BAT::ThreadManager::Update()
{
    // Check if release is request
    bool doConfig = false;
    for (auto* thread : m_threads)
    {
        if (thread->ConfigReloadInitiated())
        {
            ;
            doConfig = true;
            break;
        }
    }

    // Set config flags on all threads
    if (doConfig)
    {
        for (auto* thread : m_threads)
        {
            thread->ConfigReload();
        }
    }
}
