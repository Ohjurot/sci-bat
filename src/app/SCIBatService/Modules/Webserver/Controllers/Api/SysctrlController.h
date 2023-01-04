#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <Modules/Gateway/GatewayThread.h>

namespace SCI::BAT::Webserver::Controllers
{
    class SysctrlController : public HTTPController
    {
        public:
            // Shows status as json data
            void OnPost(const httplib::Request& request, httplib::Response& response) override;
    };
}
