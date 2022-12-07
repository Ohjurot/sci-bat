#include "GatewayThread.h"

SCI::BAT::Gateway::GatewayThread::GatewayThread(const std::shared_ptr<spdlog::logger>& gatewayLogger /*= spdlog::default_logger()*/)
{
    SetLogger(gatewayLogger);
}

int SCI::BAT::Gateway::GatewayThread::ThreadMain()
{
    using namespace std::chrono_literals;

    // Setup SMA Mappings
    NetTools::IPV4Endpoint smaEndpoint;
    SCI_ASSERT_FMT(smaEndpoint.Parse(""), "Failed to parse {} as IPv4 endpoint!", "");
    m_modbus.SetupSlave("sma", smaEndpoint, 3) // TODO: Endpoint & SlaveID
        // Map inputs
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30201, 4, 0) // U32: ENUM - Status of the device
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30775, 4, 4) // U32: FIX0 - Power
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30783, 4, 8) // U32: FIX2 - Voltage L1
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30803, 4, 12) // U32: FIX2 - Frequency
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30843, 4, 16) // U32: FIX3 - Battery Current
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30845, 4, 20) // U32: FIX0 - Battery charge
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30847, 4, 24) // U32: FIX0 - Battery capacity
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30849, 4, 28) // U32: TEMP - Battery temperature
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 30851, 4, 32) // U32: FIX2 - Battery voltage
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 31003, 4, 36) // U32: Duration - Remaining time until full charge
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 31007, 4, 40) // U32: Duration - Remaining time until full discharge
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 31057, 4, 44) // U32: ENUM - Battery status
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 40009, 4, 48) // U32: ENUM - Operation Status
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 40035, 4, 52) // U32: ENUM - Battery Type
        .Map(Modbus::Slave::RemoteMappingType::AnalogInput, 40067, 4, 56) // U32: RAW - Serial Number
        // Map outputs
        .Map(Modbus::Slave::RemoteMappingType::AnalogOutput, 40151, 4, 0) // U32: ENUM - Enable modbus power control
        .Map(Modbus::Slave::RemoteMappingType::AnalogOutput, 40149, 4, 4) // U32: FIX0 - Power Setpoint
        ;
    m_modbus
        // Alias for inputs
        .Alias("ED0", "Status") // 35=Error, 303=Off, 307=Ok, 455=Warning
        .Alias("ED4", "Power") // Power in ...
        .Alias("ED8", "Voltage") // Grid voltage in V
        .Alias("ED12", "Frequency") // Grid frequency in Hz
        .Alias("ED16", "BatteryCurrent") // Battery current in A
        .Alias("ED20", "BatteryCharge") // Charge of battery in %
        .Alias("ED24", "BatteryCapacity") // Capacity of battery in %
        .Alias("ED28", "BatteryTemperature") // Temperature of the battery in °C 
        .Alias("ED32", "BatteryVoltage") // Battery voltage in V
        .Alias("ED36", "RemainingChargeTime") // Time remaining until full charge in s
        .Alias("ED40", "RemainingDirchargeTime") // Time remaining until full charge in s
        .Alias("ED44", "BatteryStatus") // 2614=Own consumption area, 2615=State of charge conservation area, 2616=Backup power area, 2617=Deep discharge protection area, 2618=Deep discharge area
        .Alias("ED48", "OperationStatus") // 381=Stop, 1467=Start
        .Alias("ED52", "BatteryType") // 1782=Valve Regulated Lead Acid battery (VRLA), 1783=Flooded lead acid batt. (FLA), 1785=Lithium - Ion(Li - Ion)
        .Alias("ED56", "SerialNumber") // 32-Bit serial number
        // Alias for output
        .Alias("AD0", "SetPowerControlEnable") // 802=Active, 803=Inactive
        .Alias("AD4", "SetPower") // Sets power in W
        ;

    // Main loop
    while (!StopRequested())
    {
        static int rpCnt = 0;
        if (rpCnt++ == 10)
        {
            if (m_modbus.SlaveConnected("sma"))
            {
                GetLogger()->info("SMA Status: ");
            }
            rpCnt = 0;
        }

        // Update modbus IO
        m_modbus["SetPowerControlEnable"].SetDWordValue(803);
        m_modbus["SetPower"].SetDWordValue(0);
        m_modbus.IOUpdate(0.5f);

        // Wait one second
        std::this_thread::sleep_for(500ms);
    }
}

void SCI::BAT::Gateway::GatewayThread::OnStop()
{
    throw std::logic_error("The method or operation is not implemented.");
}
