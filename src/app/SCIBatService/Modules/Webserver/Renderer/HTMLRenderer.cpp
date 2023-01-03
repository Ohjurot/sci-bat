#include "HTMLRenderer.h"

SCI::BAT::Webserver::HTMLRenderer::HTMLRenderer(const std::filesystem::path& rootDir) :
    m_rootDirectory(rootDir)
{
}

std::string SCI::BAT::Webserver::HTMLRenderer::RenderTemplate(const std::filesystem::path& view, const inja::json& data /*= inja::json()*/)
{
    inja::Environment env;

    // Disk search
    auto viewPath = m_rootDirectory / view;
    SCI_ASSERT_FMT(std::filesystem::exists(viewPath), "Can't open file \"{}\"", viewPath.generic_string());

    // Create template
    GetLogger()->debug("Using view \"{}\" from file", view.generic_string());
    auto tpl = env.parse_template(viewPath.generic_string());

    // Render
    return env.render(tpl, data);
}

std::string SCI::BAT::Webserver::HTMLRenderer::RenderHTML(const std::string_view& html, const inja::json& data /*= inja::json()*/)
{
    inja::Environment env;
    return env.render(html, data);
}
