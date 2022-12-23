/*!
 * @file UqlJson.h
 * @brief Driver to read and write JSON values into the unqlite settings database.
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <SCIUtil/Exception.h>
#include <SCIUtil/Concurrent/SpinLock.h>
#include <SCIUtil/Concurrent/LockGuard.h>

#include <unqlite.h>
#include <nlohmann/json.hpp>

#include <filesystem>

namespace SCI::BAT::Config
{
    /*!
     * @brief Singleton that provided read and write access of JSON data into the settings unqlite database.
    */
    class UqlJson
    {
        // Singleton
        public:
            /*!
             * @brief Provides access to the singletons instance.
             * @return Reference to UqlJson client. 
            */
            inline static UqlJson& Get()
            {
                return s_instance;
            }

        private:
            UqlJson() = default;
            UqlJson(const UqlJson&) = delete;
            UqlJson& operator=(const UqlJson&) = delete;

            static UqlJson s_instance;

        // Class 
        public:
            ~UqlJson();

            /*!
             * @brief Initializes the client. Connect to existing database or create a new one if not exists.
             * @param dbFile Path to database file.
            */
            void Init(const std::filesystem::path& dbFile);

            /*!
             * @brief Reads a key (setting / config) form the database.
             * @param key Name of the setting to be read.
             * @param jsonOut Data that will be set when reading succeeds.
             * @return True if read succeeded.
            */
            bool ReadConfig(const std::string& key, nlohmann::json& jsonOut) const;
            /*!
             * @brief Writes a key (setting / config) form the database.
             * @param key Name of the setting to be written.
             * @param jsonIn JSON Data to be written.
             * @return True if write succeeded.
            */
            bool WriteConfig(const std::string& key, const nlohmann::json& jsonIn);
            /*!
             * @brief Delets a key (setting / config) form the database.
             * @param key Name of the setting to be deleted.
             * @return True if delete succeeded. 
            */
            bool DeleteConfig(const std::string& key);

        private:
            mutable Util::SpinLock m_lock;

            unqlite* m_db = nullptr;
    };
}
