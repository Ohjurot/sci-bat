#include "KeyboardInterrupt.h"

#include <iostream>

SCI::Util::KeyboardInterrupt SCI::Util::KeyboardInterrupt::s_instance;

void SCI::Util::KeyboardInterrupt::Register()
{
    if (!m_registered)
    {
        signal(SIGINT, &SignalHandler);
        #ifdef SIGBREAK // Win32
        signal(SIGBREAK, &SignalHandler);
        #endif
        signal(SIGABRT, &SignalHandler);
        signal(SIGTERM, &SignalHandler);
        m_registered = true;

        Get().GetLogger()->info("Registered event signals!");
    }
}

void SCI::Util::KeyboardInterrupt::SignalHandler(int signal)
{
    switch (signal)
    {
        case SIGINT:
        #ifdef SIGBREAK // Win32
        case SIGBREAK:
        #endif
        case SIGABRT:
        case SIGTERM:
            Get().GetLogger()->info("Interrupt received! ({})", signal);
            Get().m_interruptRecived = true;
            if(Get().m_callback)
            {
                Get().m_callback(signal, Get().m_callbackData);
            } 
            break;
    }
}
