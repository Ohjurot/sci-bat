#include "SysctrlController.h"

void SCI::BAT::Webserver::Controllers::SysctrlController::OnPost(const httplib::Request& request, httplib::Response& response)
{
    response.status = 401;

    nlohmann::json data;
    auto operation = request.matches[1].str();
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        if (operation == "restart")
        {
            if ((int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::Operator)
            {
                Gateway::GatewayThread::RequestSystemStop();
                response.status = 200;
            }
        }
        else
        {
            response.status = 400;
        }
    }
}
