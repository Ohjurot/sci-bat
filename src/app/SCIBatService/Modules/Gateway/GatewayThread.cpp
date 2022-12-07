#include "GatewayThread.h"

SCI::BAT::Gateway::GatewayThread::GatewayThread(const std::shared_ptr<spdlog::logger>& gatewayLogger /*= spdlog::default_logger()*/)  :
    m_modbus(64, 64)
{
    SetLogger(gatewayLogger);
    m_modbus.SetLogger(gatewayLogger);
}

int SCI::BAT::Gateway::GatewayThread::ThreadMain()
{
    using namespace std::chrono_literals;

    // TODO: Read configuration

    // Setup SMA Mappings
    NetTools::IPV4Endpoint smaEndpoint;
    SCI_ASSERT_FMT(smaEndpoint.Parse("127.0.0.1:502"), "Failed to parse \"{}\" as IPv4 endpoint!", "");

    GetLogger()->info("Creating IO-Map for SMA Inverter at {}", smaEndpoint.ToString());
    m_modbus.SetupSlave("sma", smaEndpoint, 3)
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
    while (!StopRequested())
    {
        // DEBUG: Print
        static int rpCnt = 0;
        if (rpCnt++ == 10)
        {
            if (m_modbus.SlaveConnected("sma"))
            {

                GetLogger()->info("SMA Status: {}, Power: {}W, Voltage: {}V, Freqency: {}Hz, BatteryCurrent: {}A, BatteryCharge: {}%, BatteryCapacity: {}%, BatteryTemperature: {}gC, BatteryVoltage: {}V, RemainingChargeTime: {}s, RemainingDischargeTime: {}s, BatteryStatus: {}, OperationStatus: {}, BatteryType: {}, SerialNumber: {:#08x}",
                    m_smaInputData.status, m_smaInputData.power, m_smaInputData.voltage, m_smaInputData.freqenency, m_smaInputData.batteryCurrent, m_smaInputData.batteryCharge, m_smaInputData.batteryCapacity, m_smaInputData.batteryTemperature, m_smaInputData.batteryVoltage, 
                    m_smaInputData.timeUntilFullCharge, m_smaInputData.timeUntilFullDischarge, m_smaInputData.batteryStatus, m_smaInputData.operationStaus, m_smaInputData.batteryType, static_cast<unsigned>(m_smaInputData.serialNumber));
            }
            rpCnt = 0;
        }

        // Read & write modbus values
        Util::LockGuard janitor(m_dataLock);
        SMAReadInputData(m_modbus, m_smaInputData);
        SMAWriteOutputData(m_modbus, m_smaOutputData);
        janitor.Release();

        // Update modbus IO
        spdlog::trace("Initiating gateway periodic update");
        m_modbus.IOUpdate(1.0f);

        // TODO: Process data (Modbus <--> MQTT)
        static int mdx = 0;
        mdx++;
        if (mdx == 100)
        {
            spdlog::info("Power to 100W");
            m_smaOutputData.enablePowerControle = true;
            m_smaOutputData.power = 100;
        }
        else if (mdx == 200)
        {
            spdlog::info("Power to 0W");
            m_smaOutputData.enablePowerControle = true;
            m_smaOutputData.power = 0;
        }
        else if (mdx == 300)
        {
            spdlog::info("Power to -100W");
            m_smaOutputData.enablePowerControle = true;
            m_smaOutputData.power = -100;
        }
        else if (mdx == 400)
        {
            spdlog::info("Power to 0W");
            m_smaOutputData.enablePowerControle = true;
            m_smaOutputData.power = 0;
            mdx = 0;
        }

        // Wait one second
        std::this_thread::sleep_for(1s);
    }

    return 0;
}

void SCI::BAT::Gateway::GatewayThread::OnStop()
{
    // We don't need to catch the event
}

void SCI::BAT::Gateway::GatewayThread::SMAReadInputData(Modbus::Master& modbus, SMAInData& smaIn)
{
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
