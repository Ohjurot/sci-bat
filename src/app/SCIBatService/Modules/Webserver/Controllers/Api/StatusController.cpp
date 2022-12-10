#include "StatusController.h"



void SCI::BAT::Webserver::Controllers::StatusController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        auto smaDataIn = Gateway::GatewayThread::GetInputData();
        auto smaDataOut = Gateway::GatewayThread::GetOuputData();
        std::string outData = nlohmann::json(
            {
                {
                    "battery", {
                        {"capacity", smaDataIn.batteryCapacity / 100.0f},
                        {"charge", smaDataIn.batteryCharge/ 100.0f},
                        {"current", smaDataIn.batteryCurrent},
                        {"status", fmt::format("{}", smaDataIn.batteryStatus)},
                        {"temperature", smaDataIn.batteryTemperature},
                        {"type", fmt::format("{}", smaDataIn.batteryType)},
                        {"voltage", smaDataIn.batteryVoltage},
                        {"remaining-charging-time", smaDataIn.timeUntilFullCharge},
                        {"remaining-discharging-time", smaDataIn.timeUntilFullDischarge},
                    }
                },
                {
                    "grid", {
                        {"voltage", smaDataIn.voltage},
                        {"freqency", smaDataIn.freqenency},
                    }
                },
                {
                    "inverter", {
                        {"opstatus", fmt::format("{}", smaDataIn.operationStaus)},
                        {"power", smaDataIn.power},
                        {"status", fmt::format("{}", smaDataIn.status)},
                        {"powercontrole", smaDataOut.enablePowerControle},
                        {"setpoint", smaDataOut.power},
                    }
                }                
            }
        ).dump();
        response.set_content(outData, "application/json");
    }
    else
    {
        response.status = 401;
    }
}
