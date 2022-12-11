#include "TControlThread.h"

int SCI::BAT::TControle::TControlThread::ThreadMain()
{
    using namespace std::chrono_literals;

    while (!StopRequested())
    {
        // Watchdog (value sequential write required)
        auto now = std::chrono::system_clock::now();
        if (m_watchdogExpires < now)
        {
            // Reset watchdog tripped state
            m_watchdogTriped = false;

            // Check for new MQTT message
            std::string msg;
            if(m_mailbox.GetMQTTMessage(std::filesystem::path("tcl") / "mode", msg))
            {
                // Set active mode
                bool msgValid = true;
                if (msg == "off" || msg == "0")
                {
                    // Set fan off time
                    if (m_mode == OperationMode::HeatingPwr1 || m_mode == OperationMode::HeatingPwr2 || m_mode == OperationMode::HeatingPwr3)
                    {
                        GetLogger()->info("Applying fan cooloff time");
                        m_fanOffTime = now + 1ms * m_fanCooloffTime;
                    }

                    m_modeApplyed = m_mode == OperationMode::Off;
                    m_mode = OperationMode::Off;
                }
                else if (msg == "cooling" || msg == "-1")
                {
                    m_modeApplyed = m_mode == OperationMode::Cooling;
                    m_mode = OperationMode::Cooling;
                }
                else if (msg == "heating" || msg == "h1" || msg == "1")
                {
                    m_modeApplyed = m_mode == OperationMode::HeatingPwr1;
                    m_mode = OperationMode::HeatingPwr1;
                }
                else if (msg == "h2" || msg == "2")
                {
                    m_modeApplyed = m_mode == OperationMode::HeatingPwr2;
                    m_mode = OperationMode::HeatingPwr2;
                }
                else if (msg == "h3" || msg == "3")
                {
                    m_modeApplyed = m_mode == OperationMode::HeatingPwr3;
                    m_mode = OperationMode::HeatingPwr3;
                }
                else
                {
                    msgValid = false;
                }

                if (msgValid)
                {

                    // Set watchdog
                    m_watchdogExpires = now + 15min;
                }
                else
                {
                    GetLogger()->warn("Can't interpret MQTT value \"{}\"", msg);
                }
            }

            // Update relays
            if (!m_modeApplyed)
            {
                GetLogger()->info("Appliying heating mode {}", m_mode);
                switch (m_mode)
                {
                    case OperationMode::Off:
                        SetRelais(0, false);
                        SetRelais(2, false);
                        SetRelais(3, false);
                        break;
                    case OperationMode::Cooling:
                        SetRelais(0, true);
                        SetRelais(2, false);
                        SetRelais(3, false);
                        break;
                    case OperationMode::HeatingPwr1:
                        SetRelais(0, false);
                        SetRelais(2, true);
                        SetRelais(3, false);
                        break;
                    case OperationMode::HeatingPwr2:
                        SetRelais(0, false);
                        SetRelais(2, false);
                        SetRelais(3, true);
                        break;
                    case OperationMode::HeatingPwr3:
                        SetRelais(0, false);
                        SetRelais(2, true);
                        SetRelais(3, true);
                        break;
                }

                m_modeApplyed = true;
            }
        }
        else
        {
            // Trip watchdog
            if (!m_watchdogTriped)
            {
                // All relays off
                SetRelais(0, false);
                SetRelais(2, false);
                SetRelais(3, false);

                m_watchdogTriped = true;
            }
        }

        // Update fan relay
        if (m_mode != OperationMode::HeatingPwr1 && m_mode != OperationMode::HeatingPwr2 && m_mode != OperationMode::HeatingPwr3 &&  now > m_fanOffTime && m_relaisStates[1] == true)
        {
            GetLogger()->info("Fan cooldown reached!");
            SetRelais(1, false);
        }

        // Report current state as MQTT messages
        m_mailbox.Publish(std::filesystem::path("tcontrole") / "mode", fmt::format("{}", m_mode));
        for (size_t i = 0; i < _countof(m_relaisStates); i++)
        {
            std::stringstream topic, value;
            topic << "relays/relay" << (i + 1);
            value << m_relaisStates[i];
            m_mailbox.Publish(topic.str(), value.str());
        }

        // Delay
        std::this_thread::sleep_for(1s);
    }

    // All relays off
    SetRelais(0, false);
    SetRelais(1, false);
    SetRelais(2, false);
    SetRelais(3, false);

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
                m_relaisStates[index] = true;
                std::this_thread::sleep_for(50ms);
                return true;
            }
        }
        else
        {
            if (SerialSend(m_bytesOff[index], m_bytesWordSize))
            {
                m_relaisStates[index] = false;
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
