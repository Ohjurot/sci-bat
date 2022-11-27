#include "SCIBatWebserver.h"

#include <Modules/Webserver/Controllers/Static/FossController.h>
#include <Modules/Webserver/Controllers/Static/AuthController.h>

void SCI::BAT::SCIBatWebserver::RegisterControllers()
{
    RegisterController<MyController>("/");
    RegisterController<Webserver::Controllers::FossController>("/FOSS");
    RegisterController<Webserver::Controllers::AuthController>("/auth");
}
