/*
 * Short and simple demo application that tests modbus
 * 
 * This application requires a SIEMENS log with modbus enabled
 * - The application will read I0.0 and I0.1 and output Q0.0 = I0.0 and Q0.1 = !I0.1 
 * - The application will alternate Q0.2 and Q0.3 in one second intervals
 */

#include <ModbusMaster/MSConnection.h>
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
    auto connection = RETI::Modbus::MSConnection(endpoint);

    // Simple modbus loop
    int duration = 0;
    bool xset = false;
    while (!kbInterrupt.InterupRecived())
    {
        if (!connection.Execute([&duration, &xset](RETI::Modbus::MSConnection& c)
            {
                bool inputs[8];
                c.ReadDigitalIn(0, 8, inputs);
                if (inputs[0]) xset = true;
                if (inputs[1]) xset = false;

                bool outputs[4] = { xset, !xset, duration < 10, duration >= 10 };
                c.WriteDigitalOut(0, 4, outputs);
            }
        ))
        {
            std::cout << "Connection lost!" << std::endl;
        }
        
        if (++duration == 20)
        {
            duration = 0;
        }
        Sleep(100);
    }

    // Turn all off
    connection.Execute([](RETI::Modbus::MSConnection& c) 
        {
            bool outputs[4];
            memset(outputs, 0, 4);
            c.WriteDigitalOut(0, 4, outputs);
        }
    );

    return 0;
}
