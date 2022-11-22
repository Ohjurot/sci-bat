#include "TemplateCache.h"

SCI::BAT::Webserver::TemplateCache::~TemplateCache()
{
    Util::SREWLock::WriteGuard janitor(m_lock);
    for (auto& cacheEntry : m_cache)
    {
        delete cacheEntry.second.data;
    }
}

bool SCI::BAT::Webserver::TemplateCache::Has(const std::filesystem::path& rootFile)
{
    Util::SREWLock::ReadGuard janitor(m_lock);
    
    auto itFind = m_cache.find(rootFile.generic_string());
    if (itFind != m_cache.end())
    {
        // Cache exists
        if (itFind->second.validUntil < std::chrono::system_clock::now())
        {
            // Cache is too old (evict)
            janitor.Release();
            Util::SREWLock::WriteGuard jjanitor(m_lock);
            m_cache.erase(m_cache.find(rootFile.generic_string()));
        }
        else
        {
            // Cache is existing
            return true;
        }
    }

    return false;
}

const inja::Template& SCI::BAT::Webserver::TemplateCache::Get(const std::filesystem::path& rootFile)
{
    Util::SREWLock::ReadGuard janitor(m_lock);

    SCI_ASSERT_FMT(Has(rootFile), "No cache for file \"{}\" exists!", rootFile.generic_string());
    auto it = m_cache.find(rootFile.generic_string());
    return *it->second.data;
}

void SCI::BAT::Webserver::TemplateCache::Put(const std::filesystem::path& rootFile, const inja::Template& data, size_t maxCacheAge)
{
    using namespace std::chrono_literals;

    Util::SREWLock::WriteGuard janitor(m_lock);

    auto& entry = m_cache[rootFile.generic_string()];
    if (entry.data) delete entry.data;
    entry.data = new inja::Template(data);
    entry.validUntil = std::chrono::system_clock::now() + (maxCacheAge * 1s); 
}
