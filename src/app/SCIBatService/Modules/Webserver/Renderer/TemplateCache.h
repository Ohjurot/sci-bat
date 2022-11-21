/*
 *      Cache container for template data
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <SCIUtil/Concurrent/SpinLock.h>
#include <SCIUtil/Exception.h>
#include <SCIUtil/Concurrent/LockGuard.h>

#include <inja/inja.hpp>

#include <string>
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
            };

        public:
            TemplateCache() = default;
            TemplateCache(const TemplateCache&) = delete;
            TemplateCache(TemplateCache&&) noexcept = default;

            ~TemplateCache();

            TemplateCache& operator=(const TemplateCache&) = delete;
            TemplateCache& operator=(TemplateCache&&) noexcept = default;

            bool Has(const std::filesystem::path& rootFile) const;
            const inja::Template& Get(const std::filesystem::path& rootFile) const;
            void Put(const std::filesystem::path& rootFile, const inja::Template& data);

        private:
            Util::SpinLock m_lock;
            std::unordered_map<std::string, CacheEntry> m_cache;
    };
}
