#pragma once

#include <SCIUtil/Exception.h>
#include <SCIUtil/SPDLogable.h>

#include <inja/inja.hpp>

#include <string>
#include <string_view>
#include <filesystem>

namespace SCI::BAT::Webserver
{
    class HTMLRenderer : public Util::SPDLogable
    {
        public:
            HTMLRenderer() = delete;
            HTMLRenderer(const std::filesystem::path& rootDir, size_t maxCacheAge = 60 * 60 * 24);
            HTMLRenderer(const HTMLRenderer&) = delete; 
            HTMLRenderer(HTMLRenderer&&) noexcept = default; 

            HTMLRenderer& operator=(const HTMLRenderer&) = delete;
            HTMLRenderer& operator=(HTMLRenderer&&) noexcept = delete;

            std::string RenderTemplate(const std::filesystem::path& view, const inja::json& data = inja::json());
            std::string RenderHTML(const std::string_view& html, const inja::json& data = inja::json());

        private:
            // TemplateCache m_cache; <-- no cache
            size_t m_maxCacheAge = 0;

            std::filesystem::path m_rootDirectory;
    };
}
