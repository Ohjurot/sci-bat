#include "StatusController.h"



void SCI::BAT::Webserver::Controllers::StatusController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        auto smaDataIn = Gateway::GatewayThread::GetInputData();
        auto smaDataOut = Gateway::GatewayThread::GetOuputData();
        auto tcStatus = TControle::TControlThread::GetStatus();
        std::string outData = nlohmann::json(
            {
                {
                    "battery", {
                        {"capacity", fmt::format("{}", smaDataIn.batteryCapacity / 100.0f)},
                        {"charge", fmt::format("{}", smaDataIn.batteryCharge / 100.0f)},
                        {"current", fmt::format("{}", smaDataIn.batteryCurrent)},
                        {"status", fmt::format("{}", smaDataIn.batteryStatus)},
                        {"temperature", fmt::format("{}", smaDataIn.batteryTemperature)},
                        {"type", fmt::format("{}", smaDataIn.batteryType)},
                        {"voltage", fmt::format("{}", smaDataIn.batteryVoltage)},
                        {"remaining-charging-time", fmt::format("{}", smaDataIn.timeUntilFullCharge)},
                        {"remaining-discharging-time", fmt::format("{}", smaDataIn.timeUntilFullDischarge)},
                    }
                },
                {
                    "grid", {
                        {"voltage", fmt::format("{}", smaDataIn.voltage)},
                        {"freqency", fmt::format("{}", smaDataIn.freqenency)},
                    }
                },
                {
                    "inverter", {
                        {"opstatus", fmt::format("{}", smaDataIn.operationStaus)},
                        {"power", fmt::format("{}", smaDataIn.power)},
                        {"status", fmt::format("{}", smaDataIn.status)},
                        {"powercontrole", fmt::format("{}", smaDataOut.enablePowerControle)},
                        {"setpoint", fmt::format("{}", smaDataOut.power)},
                    }
                },
                {
                    "relays", {
                        {"relay1", tcStatus.relais[0]},
                        {"relay2", tcStatus.relais[1]},
                        {"relay3", tcStatus.relais[2]},
                        {"relay4", tcStatus.relais[3]},
                    }
                },
                {
                    "tcontrol", {
                        {"mode", fmt::format("{}", tcStatus.mode)},
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
