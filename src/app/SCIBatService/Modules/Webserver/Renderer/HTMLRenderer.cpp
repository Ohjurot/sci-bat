#include "HTMLRenderer.h"

SCI::BAT::Webserver::HTMLRenderer::HTMLRenderer(const std::filesystem::path& rootDir, size_t maxCacheAge /*= 60 * 60 * 24*/) :
    // inja::Environment(m_rootDirectory.generic_string()),
    m_rootDirectory(rootDir), m_maxCacheAge(maxCacheAge)
{
}

std::string SCI::BAT::Webserver::HTMLRenderer::RenderTemplate(const std::filesystem::path& view, const inja::json& data /*= inja::json()*/)
{
    inja::Environment env;

    // Find in cache
    if (m_maxCacheAge && m_cache.Has(view))
    {
        GetLogger()->debug("Using view \"{}\" from cache", view.generic_string());
        const auto& tpl = m_cache.Get(view);
        return env.render(tpl, data);
    }
    else
    {
        // Disk search
        auto viewPath = m_rootDirectory / view;
        SCI_ASSERT_FMT(std::filesystem::exists(viewPath), "Can't open file \"{}\"", viewPath.generic_string());

        // Create template
        GetLogger()->debug("Using view \"{}\" from file", view.generic_string());
        auto tpl = env.parse_template(viewPath.generic_string());

        // Store in cache
        if (m_maxCacheAge) m_cache.Put(view, tpl, m_maxCacheAge);
        return env.render(tpl, data);
    }
}

std::string SCI::BAT::Webserver::HTMLRenderer::RenderHTML(const std::string_view& html, const inja::json& data /*= inja::json()*/)
{
    inja::Environment env;
    return env.render(html, data);
}
