/*
 * 
 * 
 * 
 * 
 */

#include <Threading/Thread.h>

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
            print(m_text);
            return 1;
        }

    private:
        std::string m_text;
};

int main()
{
    MyThread tx("x"), ty("y");
    ty.Start();
    tx.Start();

    tx.Wait();
    ty.Wait();

    if (tx.GetExecutionResult() == SCI::BAT::Thread::ExecutionResult::StoppedException) std::cout << tx.GetExceptionText() << std::endl;
    if (ty.GetExecutionResult() == SCI::BAT::Thread::ExecutionResult::StoppedException) std::cout << ty.GetExceptionText() << std::endl;
}
