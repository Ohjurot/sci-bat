#include "KeyboardInterrupt.h"

#include <iostream>

RETI::Util::KeyboardInterrupt RETI::Util::KeyboardInterrupt::s_instance;

void RETI::Util::KeyboardInterrupt::Register()
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

void RETI::Util::KeyboardInterrupt::SignalHandler(int signal)
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
