#include "SPgAccountsController.h"

void SCI::BAT::Webserver::Controllers::SPgAccountsController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    data["TITLE"] = "Configure Accounts";
    this->Section("Settings", "Accounts", data);

    // Authenticate and show view
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user && (int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::SuperAdmin)
    {
        RenderView(request, response, "static/settings/accounts", data);
    }
    else
    {
        response.status = 401;
    }
}
