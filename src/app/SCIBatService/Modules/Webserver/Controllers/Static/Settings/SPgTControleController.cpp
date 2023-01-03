#include "SPgTControleController.h"

void SCI::BAT::Webserver::Controllers::SPgTControleController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    data["TITLE"] = "Configure TControle";
    this->Section("Settings", "TControle", data);

    // Authenticate and show view
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user && (int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::Admin)
    {
        RenderView(request, response, "static/settings/tcontrole", data);
    }
    else
    {
        response.status = 401;
    }
}
