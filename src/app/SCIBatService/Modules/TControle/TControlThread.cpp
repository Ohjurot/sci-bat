#include "TControlThread.h"

int SCI::BAT::TControle::TControlThread::ThreadMain()
{
    using namespace std::chrono_literals;

    while (!StopRequested())
    {
        GetLogger()->trace("Updating relais");

        SetRelais(0, true);
        SetRelais(1, true);
        SetRelais(2, true);
        SetRelais(3, true);
        std::this_thread::sleep_for(2s);

        SetRelais(0, false);
        SetRelais(1, false);
        SetRelais(2, false);
        SetRelais(3, false);
        std::this_thread::sleep_for(2s);
    }

    return true;
}

bool SCI::BAT::TControle::TControlThread::SetRelais(unsigned int index, bool on)
{
    using namespace std::chrono_literals;

    GetLogger()->debug("Setting relais {} to {}", index, on);
    if (index < 4)
    {
        if (on)
        {
            if (SerialSend(m_bytesOn[index], m_bytesWordSize))
            {
                std::this_thread::sleep_for(50ms);
                return true;
            }
        }
        else
        {
            if (SerialSend(m_bytesOff[index], m_bytesWordSize))
            {
                std::this_thread::sleep_for(50ms);
                return true;
            }
        }
    }

    // Error 
    GetLogger()->error("Failed setting relais {} to {}", index, on);
    return false;
}

bool SCI::BAT::TControle::TControlThread::SerialSend(const void* data, unsigned int byts)
{
    if (m_serial.openDevice(m_serialDevice.c_str(), m_serialBaude, m_serialBits, m_serialParity, m_serialStopBits) == 1)
    {
        bool ok = m_serial.writeBytes(data, byts) == 1;
        m_serial.closeDevice();
        return ok;
    }

    return false;
}
