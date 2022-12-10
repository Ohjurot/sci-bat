#include "GatewayThread.h"

SCI::BAT::Gateway::GatewayThread* SCI::BAT::Gateway::GatewayThread::s_gateway = nullptr;

SCI::BAT::Gateway::GatewayThread::GatewayThread(Mailbox::MailboxThread& mailbox, const std::shared_ptr<spdlog::logger>& gatewayLogger /*= spdlog::default_logger()*/)  :
    m_modbus(64, 64),
    m_mailbox(mailbox)
{
    SetLogger(gatewayLogger);
    m_modbus.SetLogger(gatewayLogger);

    m_smaOutputData.enablePowerControle = true;

    // Activate static gateway
    s_gateway = this;
}

int SCI::BAT::Gateway::GatewayThread::ThreadMain()
{
    using namespace std::chrono_literals;

    // Read configuration
    LoadConfig();

    // Setup SMA Mappings
    NetTools::IPV4Endpoint smaEndpoint;
    std::string smaEndpointStr = fmt::format("{}:{}", m_smaIp, m_smaPort);
    SCI_ASSERT_FMT(smaEndpoint.Parse(smaEndpointStr), "Failed to parse \"{}\" as IPv4 endpoint!", smaEndpointStr);

    GetLogger()->info("Creating IO-Map for SMA Inverter at \"{}\" (Node: {})", smaEndpoint.ToString(), m_smaSlaveNode);
    m_modbus.SetupSlave("sma", smaEndpoint, m_smaSlaveNode)
        // Map inputs
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30201, 2, 0) // U32: ENUM - Status of the device
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30775, 2, 4) // U32: FIX0 - Power
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30783, 2, 8) // U32: FIX2 - Voltage L1
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30803, 2, 12) // U32: FIX2 - Frequency
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30843, 2, 16) // U32: FIX3 - Battery Current
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30845, 2, 20) // U32: FIX0 - Battery charge
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30847, 2, 24) // U32: FIX0 - Battery capacity
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30849, 2, 28) // U32: TEMP - Battery temperature
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30851, 2, 32) // U32: FIX2 - Battery voltage
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 31003, 2, 36) // U32: Duration - Remaining time until full charge
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 31007, 2, 40) // U32: Duration - Remaining time until full discharge
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 31057, 2, 44) // U32: ENUM - Battery status
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 40009, 2, 48) // U32: ENUM - Operation Status
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 40035, 2, 52) // U32: ENUM - Battery Type
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 40067, 2, 56) // U32: RAW - Serial Number
        // Map outputs
        .Map(Modbus::Slave::RemoteMappingType::AnalogOutput, 40151, 2, 0) // U32: ENUM - Enable modbus power control
        .Map(Modbus::Slave::RemoteMappingType::AnalogOutput, 40149, 2, 4) // U32: FIX0 - Power Setpoint
        ;
    m_modbus
        // Alias for inputs
        .Alias("ED 0", "Status") // 35=Error, 303=Off, 307=Ok, 455=Warning
        .Alias("ED 4", "Power") // Power in ...
        .Alias("ED 8", "Voltage") // Grid voltage in V
        .Alias("ED 12", "Frequency") // Grid frequency in Hz
        .Alias("ED 16", "BatteryCurrent") // Battery current in A
        .Alias("ED 20", "BatteryCharge") // Charge of battery in %
        .Alias("ED 24", "BatteryCapacity") // Capacity of battery in %
        .Alias("ED 28", "BatteryTemperature") // Temperature of the battery in °C 
        .Alias("ED 32", "BatteryVoltage") // Battery voltage in V
        .Alias("ED 36", "RemainingChargeTime") // Time remaining until full charge in s
        .Alias("ED 40", "RemainingDirchargeTime") // Time remaining until full charge in s
        .Alias("ED 44", "BatteryStatus") // 2614=Own consumption area, 2615=State of charge conservation area, 2616=Backup power area, 2617=Deep discharge protection area, 2618=Deep discharge area
        .Alias("ED 48", "OperationStatus") // 381=Stop, 1467=Start
        .Alias("ED 52", "BatteryType") // 1782=Valve Regulated Lead Acid battery (VRLA), 1783=Flooded lead acid batt. (FLA), 1785=Lithium - Ion(Li - Ion)
        .Alias("ED 56", "SerialNumber") // 32-Bit serial number
        // Alias for output
        .Alias("AD 0", "SetPowerControlEnable") // 802=Active, 803=Inactive
        .Alias("AD 4", "SetPower") // Sets power in W
        ;

    // Main loop
    GetLogger()->info("Starting gateway main loop");
    int dStatsCounter = 0;
    while (!StopRequested())
    {
        auto f = 4;

        // Debug stats print
        if (dStatsCounter++ == 10)
        {
            if (m_modbus.SlaveConnected("sma"))
            {

                GetLogger()->debug("SMA Status: {}, Power: {}W, PowerSetpoint: {}W, Voltage: {}V, Freqency: {}Hz, BatteryCurrent: {}A, BatteryCharge: {}%, BatteryCapacity: {}%, BatteryTemperature: {}gC, BatteryVoltage: {}V, RemainingChargeTime: {}s, RemainingDischargeTime: {}s, BatteryStatus: {}, OperationStatus: {}, BatteryType: {}, SerialNumber: {:#08x}",
                    m_smaInputData.status, m_smaInputData.power, m_smaOutputData.power, m_smaInputData.voltage, m_smaInputData.freqenency, m_smaInputData.batteryCurrent, m_smaInputData.batteryCharge, m_smaInputData.batteryCapacity, m_smaInputData.batteryTemperature, m_smaInputData.batteryVoltage, 
                    m_smaInputData.timeUntilFullCharge, m_smaInputData.timeUntilFullDischarge, m_smaInputData.batteryStatus, m_smaInputData.operationStaus, m_smaInputData.batteryType, static_cast<unsigned>(m_smaInputData.serialNumber));
            }
            dStatsCounter = 0;
        }

        // Config update
        if (ConfigReloadRequested())
        {
            GetLogger()->info("Config reload requested.");
            LoadConfig();

            GetLogger()->info("Updating modbus slave connection information");
            NetTools::IPV4Endpoint smaEndpoint;
            std::string smaEndpointStr = fmt::format("{}:{}", m_smaIp, m_smaPort);
            if (smaEndpoint.Parse(smaEndpointStr))
            {
                m_modbus.SetupSlave("sma").UpdateConnection(smaEndpoint, m_smaPort);
            }
            else
            {
                GetLogger()->error("Failed to update inverter modbus ip to \"{}\".", m_smaIp);
            }

            DoneConfigChange();
        }

        // Read & write modbus values
        Util::LockGuard janitor(m_dataLock);
        SMAReadInputData(m_modbus, m_smaInputData);
        SMAWriteOutputData(m_modbus, m_smaOutputData);
        janitor.Release();

        // Update modbus IO
        GetLogger()->debug("Initiating gateway periodic update");
        m_modbus.IOUpdate(0.0001f * m_refRateInMs);

        // Read the modbus command
        std::string mqttRequestPowerStr;
        if (m_mailbox.GetMQTTMessage(std::filesystem::path("battery") / "setpoint", mqttRequestPowerStr))
        {
            int32_t powerSetpoint = std::numeric_limits<int32_t>::max();
            std::stringstream ss;
            ss << mqttRequestPowerStr;
            ss >> powerSetpoint;

            if (powerSetpoint != std::numeric_limits<int32_t>::max() && !ss.fail())
            {
                GetLogger()->info("Power was set to {}W via MQTT", powerSetpoint);
                m_smaOutputData.enablePowerControle = true;
                m_smaOutputData.power = powerSetpoint;
            }
            else
            {
                GetLogger()->warn("Invalid MQTT input for Power Setpoint \"{}\"", mqttRequestPowerStr);
            }
        }

        // Write data to MQTT
        if (true || m_modbus.SlaveConnected("sma")) // DEBUG
        {
            PublishMQTTInfo(m_smaInputData, m_smaOutputData);
        }

        // Wait one second
        std::this_thread::sleep_for(1ms * m_refRateInMs);
    }

    // Shutdown
    GetLogger()->info("Shutdown requested! Asserting save modbus state");
    m_smaOutputData.enablePowerControle = true;
    m_smaOutputData.power = 0;
    SMAWriteOutputData(m_modbus, m_smaOutputData);
    m_modbus.IOUpdate(99999.0f); // Large number to force reconnect
    std::this_thread::sleep_for(3s);
    m_smaOutputData.enablePowerControle = false;
    m_smaOutputData.power = 0;
    SMAWriteOutputData(m_modbus, m_smaOutputData);
    m_modbus.IOUpdate(99999.0f);

    return 0;
}

void SCI::BAT::Gateway::GatewayThread::OnStop()
{
    // We don't need to catch the event
}

void SCI::BAT::Gateway::GatewayThread::PublishMQTTInfo(const SMAInData& id, const SMAOutData& od)
{
    // Battery capacity as normalized float
    if (!m_mailbox.Publish(std::filesystem::path("battery") / "capacity", fmt::format("{}", id.batteryCapacity / 100.f)))
        return;

    // Battery charge as normalized float
    if (!m_mailbox.Publish(std::filesystem::path("battery") / "charge", fmt::format("{}", id.batteryCharge / 100.f)))
        return;

    // Battery current as float
    if (!m_mailbox.Publish(std::filesystem::path("battery") / "current", fmt::format("{}", id.batteryCurrent)))
        return;

    // Battery status as string
    if (!m_mailbox.Publish(std::filesystem::path("battery") / "status", fmt::format("{}", id.batteryStatus)))
        return;

    // Battery temperature as float in °C
    if (!m_mailbox.Publish(std::filesystem::path("battery") / "temperature", fmt::format("{}", id.batteryTemperature)))
        return;

    // Battery type as string
    if (!m_mailbox.Publish(std::filesystem::path("battery") / "type", fmt::format("{}", id.batteryType)))
        return;

    // Battery voltage as float
    if (!m_mailbox.Publish(std::filesystem::path("battery") / "voltage", fmt::format("{}", id.batteryVoltage)))
        return;

    // Grid voltage as float
    if (!m_mailbox.Publish(std::filesystem::path("grid") / "voltage", fmt::format("{}", id.voltage)))
        return;

    // Grid frequency as float
    if (!m_mailbox.Publish(std::filesystem::path("grid") / "frequency", fmt::format("{}", id.freqenency)))
        return;

    // Inverter operation status as string
    if (!m_mailbox.Publish(std::filesystem::path("inverter") / "opstatus", fmt::format("{}", id.operationStaus)))
        return;

    // Inverter currently delivering / taking power
    if (!m_mailbox.Publish(std::filesystem::path("inverter") / "power", fmt::format("{}", id.power)))
        return;

    // Inverter currently controling power
    if (!m_mailbox.Publish(std::filesystem::path("inverter") / "powercontrole", fmt::format("{}", od.enablePowerControle ? "1" : "0")))
        return;

    // Inverter current setpoint
    if (!m_mailbox.Publish(std::filesystem::path("inverter") / "setpoint", fmt::format("{}", od.power)))
        return;

    // Inverter status
    if (!m_mailbox.Publish(std::filesystem::path("inverter") / "status", fmt::format("{}", id.status)))
        return;

    // Battery time until full charge
    if (!m_mailbox.Publish(std::filesystem::path("battery") / "remaining-charging-time", fmt::format("{}", id.timeUntilFullCharge)))
        return;

    // Battery time until full discharge
    if (!m_mailbox.Publish(std::filesystem::path("battery") / "remaining-discharging-time", fmt::format("{}", id.timeUntilFullDischarge)))
        return;
}

void SCI::BAT::Gateway::GatewayThread::SMAReadInputData(Modbus::Master& modbus, SMAInData& smaIn)
{
    modbus.SetSlaveEndianness(Modbus::Endianness::Big);
    smaIn.status = (SMAStatus)modbus["Status"].GetDWordValue();
    smaIn.power = modbus["Power"].GetDWordValue();
    smaIn.voltage =  SMAConvertFromFix(modbus["Voltage"].GetDWordValue(), 2);
    smaIn.freqenency =  SMAConvertFromFix(modbus["Frequency"].GetDWordValue(), 2);
    smaIn.batteryCurrent =  SMAConvertFromFix(modbus["BatteryCurrent"].GetDWordValue(), 3);
    smaIn.batteryCharge = std::clamp(modbus["BatteryCharge"].GetDWordValue(), 0, 100);
    smaIn.batteryCapacity = std::clamp(modbus["BatteryCapacity"].GetDWordValue(), 0, 100);
    smaIn.batteryTemperature =  SMAConvertFromFix(modbus["BatteryTemperature"].GetDWordValue(), 1);
    smaIn.batteryVoltage =  SMAConvertFromFix(modbus["BatteryVoltage"].GetDWordValue(), 2);
    smaIn.timeUntilFullCharge = modbus["RemainingChargeTime"].GetDWordValue();
    smaIn.timeUntilFullDischarge = modbus["RemainingDirchargeTime"].GetDWordValue();
    smaIn.batteryStatus = (SMABatteryStatus)modbus["BatteryStatus"].GetDWordValue();
    smaIn.operationStaus = (SMAOperationStatus)modbus["OperationStatus"].GetDWordValue();
    smaIn.batteryType = (SMABatteryType)modbus["BatteryType"].GetDWordValue();
    smaIn.serialNumber = modbus["SerialNumber"].GetDWordValue();
}

void SCI::BAT::Gateway::GatewayThread::SMAWriteOutputData(Modbus::Master& modbus, SMAOutData& smaOut)
{
    modbus.SetSlaveEndianness(Modbus::Endianness::Big);
    if (smaOut.enablePowerControle)
    {
        modbus["SetPowerControlEnable"].SetDWordValue(802);
        modbus["SetPower"].SetDWordValue(smaOut.power);
    }
    else
    {
        modbus["SetPowerControlEnable"].SetDWordValue(803);
    }
}

void SCI::BAT::Gateway::GatewayThread::LoadConfig()
{
    // Assert config node existence
    GetLogger()->debug("Asserting default config.");
    Config::AuthenticateConfig::InsertData(
        "inverter",
        (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Admin, (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Admin, (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::System,
        {
            { "address", "10.27.210.78" },
            { "port", 502 },
            { "node", 3 },
            { "pollrate", 3000 },
        }
    );

    // Now read current config
    nlohmann::json config;
    GetLogger()->debug("Reading config from db.");
    if (Config::AuthenticateConfig::ReadData("inverter", (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::System, config))
    {
        m_smaIp = config["address"];
        m_smaPort = config["port"];
        m_smaSlaveNode = config["node"];
        m_refRateInMs = config["pollrate"];
    }
    else
    {
        GetLogger()->error("Failed to read config.");
    }
}
