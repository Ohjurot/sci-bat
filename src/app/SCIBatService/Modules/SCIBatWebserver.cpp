#include "SCIBatWebserver.h"

#include <Modules/Webserver/Controllers/Static/IndexController.h>
#include <Modules/Webserver/Controllers/Static/FossController.h>
#include <Modules/Webserver/Controllers/Static/AuthController.h>
#include <Modules/Webserver/Controllers/Static/DashboardController.h>

#include <Modules/Webserver/Controllers/Static/Settings/SIndexController.h>
#include <Modules/Webserver/Controllers/Static/Settings/SPgSysStatusController.h>
#include <Modules/Webserver/Controllers/Static/Settings/SPgGatewayController.h>
#include <Modules/Webserver/Controllers/Static/Settings/SPgMailboxController.h>
#include <Modules/Webserver/Controllers/Static/Settings/SPgTControlController.h>
#include <Modules/Webserver/Controllers/Static/Settings/SPgAccountsController.h>

#include <Modules/Webserver/Controllers/Api/StatusController.h>
#include <Modules/Webserver/Controllers/Api/SettingsController.h>
#include <Modules/Webserver/Controllers/Api/SerialListController.h>
#include <Modules/Webserver/Controllers/Api/UsermodController.h>
#include <Modules/Webserver/Controllers/Api/SysStatusController.h>
#include <Modules/Webserver/Controllers/Api/SysctrlController.h>

void SCI::BAT::SCIBatWebserver::RegisterControllers()
{
    // Static "user" pages
    RegisterController<Webserver::Controllers::IndexController>("/");
    RegisterController<Webserver::Controllers::FossController>("/FOSS");
    RegisterController<Webserver::Controllers::AuthController>("/auth");
    RegisterController<Webserver::Controllers::DashboardController>("/Dashboard");

    // Static "admin" pages
    RegisterController<Webserver::Controllers::SIndexController>("/Settings");
    RegisterController<Webserver::Controllers::SPgSysStatusController>("/Settings/Status");
    RegisterController<Webserver::Controllers::SPgGatewayController>("/Settings/Gateway");
    RegisterController<Webserver::Controllers::SPgMailboxController>("/Settings/Mailbox");
    RegisterController<Webserver::Controllers::SPgTControlController>("/Settings/TControl");
    RegisterController<Webserver::Controllers::SPgAccountsController>("/Settings/Accounts");

    // API
    RegisterController<Webserver::Controllers::StatusController>("/api/status");
    RegisterController<Webserver::Controllers::SettingsController>("/api/setting/(\\w+)"); /* /api/settings/<setting> */
    RegisterController<Webserver::Controllers::SerialListController>("/api/serial/list");
    RegisterController<Webserver::Controllers::UsermodController>("/api/usermod/(\\w+)/(\\w+)"); /* /api/usermod/<operation>/<username> */
    RegisterController<Webserver::Controllers::SysStatusController>("/api/sysstatus");
    RegisterController<Webserver::Controllers::SysctrlController>("/api/sysctrl/(\\w+)"); /* /api/sysctrl/<operation> */
}
