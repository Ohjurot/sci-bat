#include "AuthController.h"

void SCI::BAT::Webserver::Controllers::AuthController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    inja::json data;
    data["TITLE"] = "Login";

    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        Redirect(request, response, "/");
    }
    else
    {
        data["LOGIN_FAILED"] = request.has_param("failed");
        data["LOGIN_USERNAME"] = data["LOGIN_FAILED"] ? request.get_param_value("failed") : "";

        Section("Login", data);
        RenderView(request, response, "static/login", data);
    }
}

void SCI::BAT::Webserver::Controllers::AuthController::OnPost(const httplib::Request& request, httplib::Response& response)
{
    if (request.has_param("destroy"))
    {
        OnDelete(request, response);
    }
    else
    {
        if(!request.has_param("username") ||!request.has_param("password"))
            throw std::logic_error("Invalid post request!");

        // Get username, password and compute hash
        auto username = request.get_param_value("username");
        auto password = request.get_param_value("password");

        // Get config for username
        nlohmann::json userConfig;
        Config::AuthenticateConfig::ReadData("user." + username, (int)HTTPUser::PermissionLevel::System, userConfig);

        // Validate password
        if (userConfig["enabled"].get<bool>() && HTTPAuthentication::CheckPassword(userConfig["password"], password))
        {
            inja::json data;
            HTTPAuthentication::Create(request, response, data, username, (HTTPUser::PermissionLevel)userConfig["authlevel"].get<int>());
            Redirect(request, response, "/");
        }
        else
        {
            Redirect(request, response, "/auth?failed=" + username);
        }
    }
}

void SCI::BAT::Webserver::Controllers::AuthController::OnDelete(const httplib::Request& request, httplib::Response& response)
{
    inja::json data;
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        HTTPAuthentication::Destroy(user);
    }

    Redirect(request, response, "/auth");
}
