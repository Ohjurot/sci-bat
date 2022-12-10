#include "SCIBatWebserver.h"

#include <Modules/Webserver/Controllers/Static/IndexController.h>
#include <Modules/Webserver/Controllers/Static/FossController.h>
#include <Modules/Webserver/Controllers/Static/AuthController.h>
#include <Modules/Webserver/Controllers/Static/DashboardController.h>
#include <Modules/Webserver/Controllers/Api/StatusController.h>

void SCI::BAT::SCIBatWebserver::RegisterControllers()
{
    RegisterController<Webserver::Controllers::IndexController>("/");
    RegisterController<Webserver::Controllers::FossController>("/FOSS");
    RegisterController<Webserver::Controllers::AuthController>("/auth");
    RegisterController<Webserver::Controllers::DashboardController>("/Dashboard");
    RegisterController<Webserver::Controllers::StatusController>("/api/status");
}
