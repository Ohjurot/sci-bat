#include "FossController.h"

void SCI::BAT::Webserver::Controllers::FossController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    inja::json data;
    data["TITLE"] = "FOSS";

    auto user = HTTPAuthentication::Session(request, data);

    Section("Foss", data);
    RenderView(request, response, "static/foss", data);
}
