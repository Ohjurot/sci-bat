#include "SerialListController.h"

void SCI::BAT::Webserver::Controllers::SerialListController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user && (int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::Admin)
    {
        auto serialDevices = TControle::TControlThread::ListSerialDevices();

        nlohmann::json serialDevicesJson = { { "devices", serialDevices } };
        RenderJSON(response, serialDevicesJson);
    }
    else
    {
        response.status = 401;
    }
}
