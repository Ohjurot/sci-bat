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
        auto passwordHash = HTTPAuthentication::HashPassword("admin");

        if (username == "admin" && HTTPAuthentication::CheckPassword(passwordHash, password))
        {
            inja::json data;
            HTTPAuthentication::Create(request, response, data, username, SCI::BAT::Webserver::HTTPUser::PermissionLevel::Admin);
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
