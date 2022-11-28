#include "AuthenticatedConfig.h"

nlohmann::json SCI::BAT::Config::AuthenticateConfig::DataToJson(const Data& data)
{
    nlohmann::json out;
    out["permission"] = {
        { "read", data.minReadPermissionLevel },
        { "write", data.minWritePermissionLevel },
        { "delete", data.minDeletePermissionLevel },
    };
    out["data"] = data.configData;

    return out;
}

SCI::BAT::Config::AuthenticateConfig::Data SCI::BAT::Config::AuthenticateConfig::JsonToData(const nlohmann::json& json)
{
    Data data;
    data.configData = json["data"];
    data.minReadPermissionLevel = json["permission"]["read"];
    data.minWritePermissionLevel = json["permission"]["write"];
    data.minDeletePermissionLevel = json["permission"]["delete"];
    return data;
}

bool SCI::BAT::Config::AuthenticateConfig::WriteData(const std::string& key, int currentPermissionLevel, const nlohmann::json& data)
{
    nlohmann::json oldJsonData;
    if (UqlJson::Get().ReadConfig(key, oldJsonData))
    {
        Data oldData = JsonToData(oldJsonData);

        if (oldData.minWritePermissionLevel <= currentPermissionLevel)
        {
            oldData.configData = data;

            nlohmann::json newJsonData = DataToJson(oldData);
            return UqlJson::Get().WriteConfig(key, newJsonData);
        }
    }

    return false;
}
