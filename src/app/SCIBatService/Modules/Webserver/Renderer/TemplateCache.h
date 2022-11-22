/*
 *      Cache container for template data
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <SCIUtil/Exception.h>
#include <SCIUtil/Concurrent/SREWLock.h>

#include <inja/inja.hpp>

#include <string>
#include <chrono>
#include <filesystem>
#include <unordered_map>

namespace SCI::BAT::Webserver
{
    class TemplateCache
    {
        private:
            struct CacheEntry
            {
                inja::Template* data = nullptr;
                std::chrono::system_clock::time_point validUntil;
            };

        public:
            TemplateCache() = default;
            TemplateCache(const TemplateCache&) = delete;
            TemplateCache(TemplateCache&&) noexcept = default;

            ~TemplateCache();

            TemplateCache& operator=(const TemplateCache&) = delete;
            TemplateCache& operator=(TemplateCache&&) noexcept = default;

            bool Has(const std::filesystem::path& rootFile);
            const inja::Template& Get(const std::filesystem::path& rootFile);
            void Put(const std::filesystem::path& rootFile, const inja::Template& data, size_t maxCacheAge);

        private:
            Util::SREWLock m_lock;
            std::unordered_map<std::string, CacheEntry> m_cache;
    };
}
