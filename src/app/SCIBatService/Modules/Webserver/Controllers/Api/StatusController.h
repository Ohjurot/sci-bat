#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <Modules/Gateway/GatewayThread.h>

#include <fmt/format.h>

namespace SCI::BAT::Webserver::Controllers
{
    class StatusController : public HTTPController
    {
        public:
            // Shows status as json data
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
    };
}

