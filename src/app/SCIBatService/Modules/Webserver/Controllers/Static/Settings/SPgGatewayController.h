#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

namespace SCI::BAT::Webserver::Controllers
{
    class SPgGatewayController : public HTTPController
    {
        public:
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
    };
}
