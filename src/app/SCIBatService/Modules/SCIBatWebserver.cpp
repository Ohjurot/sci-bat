#include "SCIBatWebserver.h"

void SCI::BAT::SCIBatWebserver::RegisterControllers()
{
    RegisterController<MyController>("/");
    RegisterController<Webserver::Controllers::FossController>("/FOSS");
}
