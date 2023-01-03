#include "SPgGatewayController.h"

void SCI::BAT::Webserver::Controllers::SPgGatewayController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    data["TITLE"] = "Configure Gateway";
    this->Section("Settings", "Gateway", data);

    // Authenticate and show view
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user && (int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::Admin)
    {
        RenderView(request, response, "static/settings/gateway", data);
    }
    else
    {
        response.status = 401;
    }
}
