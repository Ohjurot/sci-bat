/*!
 * @file AuthenticatedConfig.h
 * @brief Provided access to the configuration database using the authentication scheme.
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Config/UqlJson.h>

#include <nlohmann/json.hpp>

#include <string>

namespace SCI::BAT::Config
{
    /*!
     * @brief Provided access to the configuration database using the authentication scheme.
    */
    class AuthenticateConfig
    {
        public:
            /*!
             * @brief Raw format of a stored setting value.
            */
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

            /*!
             * @brief Helper function that converts raw settings data (struct) to json.
             * @param data Reference to parsed data object.
             * @return Parsed json output.
            */
            static nlohmann::json DataToJson(const Data& data);
            /*!
             * @brief Helper function that converts raw json to a process settings value (struct).
             * @param json Input json configuration.
             * @return Parsed struct output.
            */
            static Data JsonToData(const nlohmann::json& json);

            /*!
             * @brief Tries to write setting.
             * @param key The key to which the write shall occur. The key must exist.
             * @param currentPermissionLevel The current users permission level. User needs to be allowed to write or the function will fail.
             * @param data Data to wire to the with key specified setting.
             * @return True if data was written. False if the key did not exists or the user had insufficient permission.
            */
            static bool WriteData(const std::string& key, int currentPermissionLevel, const nlohmann::json& data);
            /*!
             * @brief Tries to read setting.
             * @param key The key to which the read shall occur. The key must exist.
             * @param currentPermissionLevel The current users permission level. User needs to be allowed to read or the function will fail.
             * @param data Data to read to the with key specified setting.
             * @return True if data was read. False if the key did not exists or the user had insufficient permission.
            */
            static bool ReadData(const std::string& key, int currentPermissionLevel, nlohmann::json& data);
            /*!
             * @brief Tries to delete a setting.
             * @param key The key that should be deleted. The key must exist.
             * @param currentPermissionLevel The current users permission level. User needs to be allowed to write or the function will fail.
             * @return True if setting was delete. False if the key did not exists or the user had insufficient permission.
            */
            static bool DeleteData(const std::string& key, int currentPermissionLevel);
            /*!
             * @brief Inserts data into the settings database.
             * @param key Name of the setting to be inserted. The key must not exists.
             * @param permissionRead Read permission level of the new setting.
             * @param permissionWrite Write permission level of the new setting. 
             * @param permissionDelete Delete permission level of the new setting.
             * @param data Data to be inserted.
             * @return True if setting was inserted successfully.
            */
            static bool InsertData(const std::string& key, int permissionRead, int permissionWrite, int permissionDelete, const nlohmann::json& data);
    };
}
