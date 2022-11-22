#include "HTMLRenderer.h"

std::string SCI::BAT::Webserver::HTMLRenderer::RenderTemplate(const std::filesystem::path& view, const inja::json& data /*= inja::json()*/)
{
    // Find in cache
    if (m_maxCacheAge && m_cache.Has(view))
    {
        GetLogger()->debug("Using view \"{}\" from cache", view.generic_string());
        const auto& tpl = m_cache.Get(view);
        return render(tpl, data);
    }
    else
    {
        // Disk search
        auto viewPath = m_rootDirectory / view;
        SCI_ASSERT_FMT(std::filesystem::exists(viewPath), "Can't open file \"{}\"", viewPath.generic_string());

        // Create template
        GetLogger()->debug("Using view \"{}\" from file", view.generic_string());
        auto tpl = parse_template(viewPath.generic_string());

        // Store in cache
        if (m_maxCacheAge) m_cache.Put(view, tpl, m_maxCacheAge);
        return render(tpl, data);
    }
}

std::string SCI::BAT::Webserver::HTMLRenderer::RenderHTML(const std::string_view& html, const inja::json& data /*= inja::json()*/)
{
    return render(html, data);
}
