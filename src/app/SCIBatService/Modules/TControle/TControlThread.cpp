#include "TControlThread.h"

SCI::BAT::TControle::TControlThread* SCI::BAT::TControle::TControlThread::s_instance = nullptr;

int SCI::BAT::TControle::TControlThread::ThreadMain()
{
    using namespace std::chrono_literals;

    // Load from config
    LoadConfig();

    // Get a list of serial devices and print them
    auto serialDevices = ListSerialDevices();
    for (const auto& device : serialDevices)
        GetLogger()->debug("Found serial device \"{}\".", device);

    // Abort when no serial is available
    m_deviceAvailable = std::find(serialDevices.begin(), serialDevices.end(), m_serialDevice) != serialDevices.end();
    if(serialDevices.size() == 0)
        throw std::runtime_error("No serial devices present! TControle will NOT work!");

    // Report
    GetLogger()->info("Using serial device \"{}\". Currently detected: {}", m_serialDevice, m_deviceAvailable ? "YES" : "NO");

    // Loop
    while (!StopRequested())
    {
        // Get timestamp
        auto now = std::chrono::system_clock::now();

        // Reload config
        if (ConfigReloadRequested())
        {
            GetLogger()->info("Config change requested!");

            // Save state
            GetLogger()->info("Going info save state...");
            m_watchdogExpires = now;
            m_watchdogTriped = false;
            SetRelais(0, false);
            SetRelais(1, false);
            SetRelais(2, false);
            SetRelais(3, false);

            // Reload config
            GetLogger()->info("Reloading config.");
            LoadConfig();

            // List devices
            serialDevices = ListSerialDevices();
            for (const auto& device : serialDevices)
                GetLogger()->debug("Found serial device \"{}\".", device);

            // Validate serial devices
            if (serialDevices.size() == 0)
                throw std::runtime_error("No serial devices present! TControle will terminate!");

            // Report
            m_deviceAvailable = std::find(serialDevices.begin(), serialDevices.end(), m_serialDevice) != serialDevices.end();
            GetLogger()->info("Using serial device \"{}\". Currently detected: {}", m_serialDevice, m_deviceAvailable ? "YES" : "NO");

            // Finished
            GetLogger()->info("Finished config reload.");
            DoneConfigChange();
        }

        // Check for new MQTT message
        std::string msg;
        if (m_mailbox.GetMQTTMessage(std::filesystem::path("tcontrol") / "mode", msg))
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
                // Set fan off time
                if (m_mode == OperationMode::HeatingPwr1 || m_mode == OperationMode::HeatingPwr2 || m_mode == OperationMode::HeatingPwr3)
                {
                    GetLogger()->info("Applying fan cooloff time");
                    m_fanOffTime = now + 1ms * m_fanCooloffTime;
                }

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

        // Watchdog (value sequential write required)
        if (now < m_watchdogExpires)
        {
            // Reset watchdog tripped state
            m_watchdogTriped = false;

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
                        SetRelais(1, true);
                        SetRelais(2, true);
                        SetRelais(3, false);
                        break;
                    case OperationMode::HeatingPwr2:
                        SetRelais(0, false);
                        SetRelais(1, true);
                        SetRelais(2, false);
                        SetRelais(3, true);
                        break;
                    case OperationMode::HeatingPwr3:
                        SetRelais(0, false);
                        SetRelais(1, true);
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
        m_mailbox.Publish(std::filesystem::path("tcontrol") / "mode", fmt::format("{}", m_mode));
        for (size_t i = 0; i < 4; i++)
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

void SCI::BAT::TControle::TControlThread::LoadConfig()
{
    // Assert config node existence
    GetLogger()->debug("Asserting default config.");
    Config::AuthenticateConfig::InsertData(
        "tcontrole",
        (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Admin, (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Admin, (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::System,
        {
            {
                "serial",
                {
                    { "device", "/dev/tty" },
                }
            },
            { "cooloff-time", 5000 }
        }
    );

    // Now read current config
    nlohmann::json config;
    GetLogger()->debug("Reading config from db.");
    if (Config::AuthenticateConfig::ReadData("tcontrole", (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::System, config))
    {
        m_serialDevice = config["serial"]["device"];
        m_fanCooloffTime = config["cooloff-time"];
    }
    else
    {
        GetLogger()->error("Failed to read config.");
    }
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
    bool ok = false; 
    if (m_serial.openDevice(m_serialDevice.c_str(), m_serialBaude, m_serialBits, m_serialParity, m_serialStopBits) == 1)
    {
        ok = m_serial.writeBytes(data, byts) == 1;
        m_serial.closeDevice();
    }
    std::atomic_thread_fence(std::memory_order::acquire); // Flush all previous write before accessing stored value... no more sync needed
    
    m_lastCommandOk = ok;
    return ok;
}

std::vector<std::string> SCI::BAT::TControle::TControlThread::ListSerialDevices()
{
    std::vector<std::string> serialPorts;
#if defined(SCI_WINDOWS)
    HKEY serialRootKey = 0;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &serialRootKey) != ERROR_SUCCESS)
        throw std::runtime_error("Failed to open registry key!");

    int index = 0;
    while (true)
    {
        char nameBuffer[1024], valueBuffer[1024];
        DWORD nameLen = 1024, valueLen = 1024;
        DWORD valueType;
        auto res = RegEnumValueA(serialRootKey, index++, nameBuffer, &nameLen, nullptr, &valueType, (BYTE*)valueBuffer, &valueLen);
        if (res == ERROR_NO_MORE_ITEMS)
            break;
        if (res != ERROR_SUCCESS)
            break;

        if(valueType == REG_SZ)
            serialPorts.push_back(valueBuffer);
    }
    RegCloseKey(serialRootKey);

    std::filesystem::path p;
#elif defined(SCI_LINUX)
    std::filesystem::path serialRoot("/sys/class/tty");
    for (auto const& dir_entry : std::filesystem::directory_iterator(serialRoot))
    {
        if (dir_entry.is_directory())
        {
            auto serialPath = dir_entry.path();
            if (std::filesystem::exists(serialPath / "device"))
            {
                auto serialName = serialPath.filename();
                auto devicePath = std::filesystem::path("/dev") / serialName;
                serialPorts.push_back(devicePath.string());
            }
        }
    }
#else
    static_assert(false, "Not implemented!");
#endif
    return serialPorts;
}
