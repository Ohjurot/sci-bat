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
            Get().m_interruptRecived = true;
            Get().GetLogger()->info("Interrupt received! ({})", signal);
            break;
    }
}
