#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <Config/AuthenticatedConfig.h>

#include <fmt/format.h>

namespace SCI::BAT::Webserver::Controllers
{
    class UsermodController : public HTTPController
    {
        public:
            // Checks a user
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
            // Modifies a user
            void OnPost(const httplib::Request& request, httplib::Response& response) override;
    };
}
