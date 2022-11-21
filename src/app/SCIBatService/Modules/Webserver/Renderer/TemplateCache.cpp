#include "TemplateCache.h"



SCI::BAT::Webserver::TemplateCache::~TemplateCache()
{
    for (auto& cacheEntry : m_cache)
    {
        delete cacheEntry.second.data;
    }
}

bool SCI::BAT::Webserver::TemplateCache::Has(const std::filesystem::path& rootFile) const
{
    return m_cache.find(rootFile.generic_string()) != m_cache.end();
}

const inja::Template& SCI::BAT::Webserver::TemplateCache::Get(const std::filesystem::path& rootFile) const
{
    SCI_ASSERT_FMT(Has(rootFile), "No cache for file \"{}\" exists!", rootFile.generic_string());
    auto it = m_cache.find(rootFile.generic_string());
    return *it->second.data;
}

void SCI::BAT::Webserver::TemplateCache::Put(const std::filesystem::path& rootFile, const inja::Template& data)
{
    Util::LockGuard janitor(m_lock);

    auto** tplc = &m_cache[rootFile.generic_string()].data;
    if (*tplc) delete* tplc;
    *tplc = new inja::Template(data);
}
