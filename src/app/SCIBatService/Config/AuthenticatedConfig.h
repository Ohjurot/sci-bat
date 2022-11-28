#pragma once

#include <Config/UqlJson.h>

#include <nlohmann/json.hpp>

#include <string>

namespace SCI::BAT::Config
{
    class AuthenticateConfig
    {
        public:
            struct Data
            {
                int minReadPermissionLevel = -1;
                int minWritePermissionLevel = -1;
                int minDeletePermissionLevel = -1;

                nlohmann::json configData;
            };

        public:
            AuthenticateConfig() = delete;
            AuthenticateConfig(const AuthenticateConfig&) = delete;
            AuthenticateConfig& operator=(const AuthenticateConfig&) = delete;

            static nlohmann::json DataToJson(const Data& data);
            static Data JsonToData(const nlohmann::json& json);

            static bool WriteData(const std::string& key, int currentPermissionLevel, const nlohmann::json& data);
            static bool ReadData(const std::string& key, int currentPermissionLevel, const nlohmann::json& data);
            static bool DeleteData(const std::string& key, int currentPermissionLevel, const nlohmann::json& data);
            static bool InsertData(const std::string& key, int permissionRead, int permissionWrite, int permissionDelete, const nlohmann::json& data);
    };
}
