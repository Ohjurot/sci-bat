#include "SPgSysStatusController.h"

void SCI::BAT::Webserver::Controllers::SPgSysStatusController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    data["TITLE"] = "Configure Status";
    this->Section("Settings", "Status", data);

    // Authenticate and show view
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user && (int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::Operator)
    {
        RenderView(request, response, "static/settings/status", data);
    }
    else
    {
        response.status = 401;
    }
}
