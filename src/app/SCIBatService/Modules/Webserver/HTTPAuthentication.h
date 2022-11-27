#pragma once

#include <httplib/httplib.h>
#include <inja/inja.hpp>

#include <string>

namespace SCI::BAT::Webserver
{
    struct HTTPUser
    {
        enum class PermissionLevel : int
        {
            Unauthenticated = -1,
            Viewer = 0,
            Operator = 1,
            Admin = 2,
            SuperAdmin = 10,
        };

        std::string name = "";
        PermissionLevel permissionLevel = PermissionLevel::Unauthenticated;
    };

    class HTTPAuthentication
    {
        public:
            static HTTPUser Session(const httplib::Request& request, inja::json& data);
    };
}
