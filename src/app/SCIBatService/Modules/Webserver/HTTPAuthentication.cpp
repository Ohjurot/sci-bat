#include "HTTPAuthentication.h"

SCI::BAT::Webserver::HTTPUser SCI::BAT::Webserver::HTTPAuthentication::Session(const httplib::Request& request, inja::json& data)
{
    // Quick and dirty session
    HTTPUser user;
    if (request.has_header("Cookie"))
    {
        std::string cookieHeader = (*request.headers.find("Cookie")).second;
        auto eqPos = cookieHeader.find_first_of('=');
        if (eqPos != std::string::npos)
        {
            std::string cookieName = cookieHeader.substr(0, eqPos);
            std::string cookieValue = cookieHeader.substr(eqPos + 1);
            if (cookieName == "SCI_BAT_AUTH")
            {
                // TODO: Auth lookup
            }
        }
    }

    // Populate json
    data["USERNAME"] = user.name;
    data["AUTH_LEVEL"] = (int)user.permissionLevel;

    return user;
}
