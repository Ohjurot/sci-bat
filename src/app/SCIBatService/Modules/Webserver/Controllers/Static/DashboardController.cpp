#include "DashboardController.h"

void SCI::BAT::Webserver::Controllers::DashboardController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    data["TITLE"] = "Dashboard";
    this->Section("Dashboard", data);

    // Authenticate and show view
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        RenderView(request, response, "static/dashboard", data);
    }
    else
    {
        response.set_redirect("/auth");
    }
}
