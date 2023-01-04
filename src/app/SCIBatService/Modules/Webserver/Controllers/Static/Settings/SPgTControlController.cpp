#include "SPgTControlController.h"

void SCI::BAT::Webserver::Controllers::SPgTControlController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    data["TITLE"] = "Configure TControl";
    this->Section("Settings", "TControl", data);

    // Authenticate and show view
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user && (int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::Admin)
    {
        RenderView(request, response, "static/settings/tcontrol", data);
    }
    else
    {
        response.status = 401;
    }
}
