#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <Modules/Gateway/GatewayThread.h>
#include <Modules/TControle/TControlThread.h>

#include <fmt/format.h>

namespace SCI::BAT::Webserver::Controllers
{
    class SettingsController : public HTTPController
    {
        public:
            // Show current settings
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
            // Update settings
            void OnPost(const httplib::Request& request, httplib::Response& response) override;
    };
}

