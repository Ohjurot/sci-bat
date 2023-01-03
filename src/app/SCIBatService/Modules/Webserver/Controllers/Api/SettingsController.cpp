#include "SettingsController.h"

void SCI::BAT::Webserver::Controllers::SettingsController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    auto setting = request.matches[1];
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        nlohmann::json config;
        if (Config::AuthenticateConfig::ReadData(setting.str(), (int)user.permissionLevel, config))
        {
            RenderJSON(response, config);
        }
    }
    else
    {
        response.status = 401;
    }
}

void SCI::BAT::Webserver::Controllers::SettingsController::OnPost(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    auto setting = request.matches[1];
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        nlohmann::json config = nlohmann::json::parse(request.body);
        if (Config::AuthenticateConfig::WriteData(setting.str(), (int)user.permissionLevel, config))
        {
            GetLogger()->info("Audit: User \"{}\" wrote to config node \"{}\".", user.name, setting.str());
            Gateway::GatewayThread::ReloadAllModules();
        }
        else
        {
            GetLogger()->warn("Audit: User \"{}\" failed writing to config node \"{}\".", user.name, setting.str());
        }
    }
    else
    {
        response.status = 401;
    }
}
