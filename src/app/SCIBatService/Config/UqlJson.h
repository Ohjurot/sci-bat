#pragma once

#include <SCIUtil/Exception.h>
#include <SCIUtil/Concurrent/SpinLock.h>
#include <SCIUtil/Concurrent/LockGuard.h>

#include <unqlite.h>
#include <nlohmann/json.hpp>

#include <filesystem>

namespace SCI::BAT::Config
{
    class UqlJson
    {
        // Singleton
        public:
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

            void Init(const std::filesystem::path& dbFile);

            bool ReadConfig(const std::string& key, nlohmann::json& jsonOut) const;
            bool WriteConfig(const std::string& key, const nlohmann::json& jsonIn);
            bool DeleteConfig(const std::string& key);

        private:
            mutable Util::SpinLock m_lock;

            unqlite* m_db = nullptr;
    };
}
