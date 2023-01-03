#include "SPgMailboxController.h"

void SCI::BAT::Webserver::Controllers::SPgMailboxController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    data["TITLE"] = "Configure Mailbox";
    this->Section("Settings", "Mailbox", data);

    // Authenticate and show view
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user && (int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::Admin)
    {
        RenderView(request, response, "static/settings/mailbox", data);
    }
    else
    {
        response.status = 401;
    }
}
