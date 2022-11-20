/*
 * 
 * 
 * 
 * 
 */

#include <Threading/Thread.h>
#include <Threading/ThreadManager.h>

#include <SCIUtil/KeyboardInterrupt.h>
#include <SCIUtil/Concurrent/SpinLock.h>
#include <SCIUtil/Concurrent/LockGuard.h>

#include <iostream>
#include <string>
#include <string_view>

void print(const std::string_view& text)
{
    static SCI::Util::SpinLock lock;
    SCI::Util::LockGuard janitor(lock);
    std::cout << text << std::endl;
}

class MyThread : public SCI::BAT::Thread
{
    public:
        MyThread(const std::string_view& text) : 
            m_text(text)
        {}

    protected:
        int ThreadMain() override
        {
            while (!StopRequested())
            {
                print(m_text);
            }

            return 1;
        }

    private:
        std::string m_text;
};

int main()
{
    auto& kbInterrupt = SCI::Util::KeyboardInterrupt::Get();
    kbInterrupt.Register();

    MyThread tx("Work X"), ty("Work Y");
    
    SCI::BAT::ThreadManager tmgr;
    tmgr << tx << ty;
    tmgr.Start();

    while (tmgr())
    {
        if (kbInterrupt.InterupRecived())
        {
            tmgr.Stop();
        }
        std::this_thread::yield();
    }

    tmgr.Wait();

    if (tx.GetExecutionResult() == SCI::BAT::Thread::ExecutionResult::StoppedException) std::cout << tx.GetExceptionText() << std::endl;
    if (ty.GetExecutionResult() == SCI::BAT::Thread::ExecutionResult::StoppedException) std::cout << ty.GetExceptionText() << std::endl;
}
