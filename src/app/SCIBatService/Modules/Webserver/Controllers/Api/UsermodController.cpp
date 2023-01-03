#include "UsermodController.h"

void SCI::BAT::Webserver::Controllers::UsermodController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    response.status = 401;

    nlohmann::json data;
    auto operation = request.matches[1].str();
    auto targetuser = request.matches[2].str();
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        const auto jsonKey = std::string("user.") + targetuser;
        
        // Check account status
        if (operation == "status")
        {
            nlohmann::json jsonConfig;
            if (Config::AuthenticateConfig::ReadData(jsonKey, (int)user.permissionLevel, jsonConfig))
            {
                nlohmann::json jsonResponse = { { "active", jsonConfig["enabled"].get<bool>() && !jsonConfig["password"].get<std::string>().empty()} };
                response.status = 200;
                RenderJSON(response, jsonResponse);
            }
        }
        else
        {
            response.status = 400;
        }
    }
}

void SCI::BAT::Webserver::Controllers::UsermodController::OnPost(const httplib::Request& request, httplib::Response& response)
{
    response.status = 401;

    nlohmann::json data;
    auto operation = request.matches[1].str();
    auto targetuser = request.matches[2].str();
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user)
    {
        const auto jsonKey = std::string("user.") + targetuser;
        
        // Enable account
        if (operation == "activate")
        {
            nlohmann::json jsonConfig;
            if (Config::AuthenticateConfig::ReadData(jsonKey, (int)user.permissionLevel, jsonConfig))
            {
                // Validate password
                if (
                    request.body.length() >= 8 &&
                    request.body.length() <= 32 &&
                    request.body.find_first_of("1234567890") != std::string::npos &&
                    request.body.find_first_of(",.:-_#+~<>!§$%&(){}=?@") != std::string::npos &&
                    request.body.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos &&
                    request.body.find_first_not_of("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.:-_#+~<>!§$%&(){}=?@") == std::string::npos
                    )
                {
                    jsonConfig["enabled"] = true;
                    jsonConfig["password"] = HTTPAuthentication::HashPassword(request.body);
                    if (Config::AuthenticateConfig::WriteData(jsonKey, (int)user.permissionLevel, jsonConfig))
                    {
                        GetLogger()->info("Audit: Password for account \"{}\" changed. Account was activated.", targetuser);
                        response.status = 200;
                    }
                }
                else
                {
                    response.status = 400;
                }                
            }
        }
        // Disable account
        else if (operation == "deactivate" && targetuser != "superadmin")
        {
            nlohmann::json jsonConfig;
            if (Config::AuthenticateConfig::ReadData(jsonKey, (int)user.permissionLevel, jsonConfig))
            {
                jsonConfig["enabled"] = false;
                jsonConfig["password"] = "";
                if (Config::AuthenticateConfig::WriteData(jsonKey, (int)user.permissionLevel, jsonConfig))
                {
                    GetLogger()->info("Audit: Account \"{}\" was deactivate.", targetuser);
                    response.status = 200;
                }
            }
        }
        else
        {
            response.status = 400;
        }
    }
}
