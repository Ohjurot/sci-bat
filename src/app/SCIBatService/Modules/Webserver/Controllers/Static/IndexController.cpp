#include "IndexController.h"

void SCI::BAT::Webserver::Controllers::IndexController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    data["TITLE"] = "Home";
    this->Section(data);

    // Authenticate and show view
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        RenderView(request, response, "static/home_user", data);
    }
    else
    {
        RenderView(request, response, "static/home_guest", data);
    }
}
