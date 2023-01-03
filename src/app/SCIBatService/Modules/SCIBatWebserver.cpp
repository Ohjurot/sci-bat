#include "SCIBatWebserver.h"

#include <Modules/Webserver/Controllers/Static/IndexController.h>
#include <Modules/Webserver/Controllers/Static/FossController.h>
#include <Modules/Webserver/Controllers/Static/AuthController.h>
#include <Modules/Webserver/Controllers/Static/DashboardController.h>

#include <Modules/Webserver/Controllers/Static/Settings/SIndexController.h>
#include <Modules/Webserver/Controllers/Static/Settings/SPgGatewayController.h>
#include <Modules/Webserver/Controllers/Static/Settings/SPgMailboxController.h>
#include <Modules/Webserver/Controllers/Static/Settings/SPgTControleController.h>
#include <Modules/Webserver/Controllers/Static/Settings/SPgAccountsController.h>

#include <Modules/Webserver/Controllers/Api/StatusController.h>
#include <Modules/Webserver/Controllers/Api/SettingsController.h>
#include <Modules/Webserver/Controllers/Api/SerialListController.h>
#include <Modules/Webserver/Controllers/Api/UsermodController.h>

void SCI::BAT::SCIBatWebserver::RegisterControllers()
{
    // Static "user" pages
    RegisterController<Webserver::Controllers::IndexController>("/");
    RegisterController<Webserver::Controllers::FossController>("/FOSS");
    RegisterController<Webserver::Controllers::AuthController>("/auth");
    RegisterController<Webserver::Controllers::DashboardController>("/Dashboard");

    // Static "admin" pages
    RegisterController<Webserver::Controllers::SIndexController>("/Settings");
    RegisterController<Webserver::Controllers::SPgGatewayController>("/Settings/Gateway");
    RegisterController<Webserver::Controllers::SPgMailboxController>("/Settings/Mailbox");
    RegisterController<Webserver::Controllers::SPgTControleController>("/Settings/TControle");
    RegisterController<Webserver::Controllers::SPgAccountsController>("/Settings/Accounts");

    // API
    RegisterController<Webserver::Controllers::StatusController>("/api/status");
    RegisterController<Webserver::Controllers::SettingsController>("/api/setting/(\\w+)"); /* /api/settings/<setting> */
    RegisterController<Webserver::Controllers::SerialListController>("/api/serial/list");
    RegisterController<Webserver::Controllers::UsermodController>("/api/usermod/(\\w+)/(\\w+)"); /* /api/usermod/<operation>/<username> */
}
