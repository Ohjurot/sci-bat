#include "SIndexController.h"

void SCI::BAT::Webserver::Controllers::SIndexController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    data["TITLE"] = "Settings";
    this->Section("Settings", data);

    // Authenticate and show view
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user && (int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::Operator)
    {
        RenderView(request, response, "static/settings/index", data);
    }
    else
    {
        response.status = 401;
    }
}
