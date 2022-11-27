#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

namespace SCI::BAT::Webserver::Controllers
{
    class AuthController : public HTTPController
    {
        public:
            // Show login form
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
            // Try sign in
            void OnPost(const httplib::Request& request, httplib::Response& response) override;
            // Sign out
            void OnDelete(const httplib::Request& request, httplib::Response& response) override;
    };
}

