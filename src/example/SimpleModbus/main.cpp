/*
 * Short and simple demo application that tests modbus
 * 
 * This application requires a SIEMENS log with modbus enabled
 * - The application will read I0.0 and I0.1 and output Q0.0 = I0.0 and Q0.1 = !I0.1 
 * - The application will alternate Q0.2 and Q0.3 in one second intervals
 */

#include <ModbusMaster/Master.h>
#include <RETIUtil/KeyboardInterrupt.h>

#include <iostream>
#include <string>


int main(int argc, char** argv)
{
    // Prompt for IP and port
    RETI::NetTools::IPV4Endpoint endpoint;
    if (!scn::prompt("Please enter the LOGOs IP Address (Example 192.168.0.20:502): ", "{}", endpoint))
    {
        fmt::print("Invalid IP/Port format!");
        return -1;
    }

    // Register interrupts
    auto& kbInterrupt = RETI::Util::KeyboardInterrupt::Get();
    kbInterrupt.Register();

    // Connection
    auto modbus = RETI::Modbus::Master(32, 32);
    modbus.SetupSlave("logo", endpoint)
        .Map(RETI::Modbus::Slave::RemoteMappingType::DigitalInput, 0, 8, 0)
        .Map(RETI::Modbus::Slave::RemoteMappingType::DigitalOutput, 0, 4, 0);

    modbus.At("I 5.2");

    // Simple modbus loop
    int duration = 0;
    bool xset = false;
    while (!kbInterrupt.InterupRecived())
    {
        // Update modbus
        modbus.IOUpdate(0.1f);

        // IO
        if (modbus["I 0.0"]) xset = true;
        if (modbus["I 0.1"]) xset = false;
        modbus["O 0.0"] = xset;
        modbus["O 0.1"] = !xset;
        modbus["O 0.2"] = duration < 10;
        modbus["O 0.3"] = duration >= 10;

        // Time
        if (++duration == 20)
        {
            duration = 0;
        }

        Sleep(100);
    }

    // Turn all off
    modbus.GetProcessImage().AllOutputsLow();
    modbus.IOUpdate(0);

    return 0;
}
