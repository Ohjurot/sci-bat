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
    }
}

void RETI::Util::KeyboardInterrupt::SignalHandler(int signal)
{
    switch (signal)
    {
        case SIGINT:
            __fallthrough;
        case SIGBREAK:
            __fallthrough;
        case SIGABRT:
            __fallthrough;
        case SIGTERM:
            Get().m_interruptRecived = true;
            break;
    }
}
