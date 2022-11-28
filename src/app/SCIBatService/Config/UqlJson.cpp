#include "UqlJson.h"

SCI::BAT::Config::UqlJson SCI::BAT::Config::UqlJson::s_instance;

SCI::BAT::Config::UqlJson::~UqlJson()
{
    if (m_db)
    {
        unqlite_close(m_db);
    }
}

void SCI::BAT::Config::UqlJson::Init(const std::filesystem::path& dbFile)
{
    int rc = unqlite_open(&m_db, dbFile.generic_string().c_str(), UNQLITE_OPEN_CREATE);
    SCI_ASSERT_FMT(rc == UNQLITE_OK, "Failed to opern config datable \"{}\" (Error code {})", dbFile.generic_string(), rc);
}

bool SCI::BAT::Config::UqlJson::ReadConfig(const std::string& key, nlohmann::json& jsonOut) const
{
    SCI_ASSERT(m_db, "Config database not initialized");


    Util::LockGuard janitor(m_lock); // Begin critical section
    unqlite_int64 dataLen = 0;
    if (unqlite_kv_fetch(m_db, key.c_str(), key.length(), nullptr, &dataLen) == UNQLITE_OK)
    {
        std::string jsonData;
        jsonData.resize(dataLen + 1);
        
        if (unqlite_kv_fetch(m_db, key.c_str(), key.length(), jsonData.data(), &dataLen) == UNQLITE_OK)
        {
            jsonOut = nlohmann::json::parse(jsonData);
            return !jsonOut.empty();
        }
    }

    return false;
}

bool SCI::BAT::Config::UqlJson::WriteConfig(const std::string& key, const nlohmann::json& jsonIn)
{
    SCI_ASSERT(m_db, "Config database not initialized");
    SCI_ASSERT(!jsonIn.empty(), "Can't store empty json data!");

    std::string jsonData = nlohmann::to_string(jsonIn);

    Util::LockGuard janitor(m_lock); // Begin critical section
    return unqlite_kv_store(m_db, key.c_str(), key.length(), jsonData.c_str(), jsonData.length()) == UNQLITE_OK;
}

bool SCI::BAT::Config::UqlJson::DeleteConfig(const std::string& key)
{
    SCI_ASSERT(m_db, "Config database not initialized");

    Util::LockGuard janitor(m_lock); // Begin critical section
    return unqlite_kv_delete(m_db, key.c_str(), key.length()) == UNQLITE_OK;
}
