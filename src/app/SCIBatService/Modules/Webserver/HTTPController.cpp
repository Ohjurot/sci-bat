#include "HTTPController.h"

void SCI::BAT::Webserver::HTTPController::RenderMIME(httplib::Response& response, const std::string& data, const std::string& mimeType)
{
    SCI_ASSERT_FMT(m_renderer, "Render on controller \"{}\" ({}) not set", ToString(), typeid(*this).name());
    response.set_content(data, mimeType);
}

void SCI::BAT::Webserver::HTTPController::RenderMIME(httplib::Response& response, const char* data, size_t size, const std::string& mimeType)
{
    SCI_ASSERT_FMT(m_renderer, "Render on controller \"{}\" ({}) not set", ToString(), typeid(*this).name());
    response.set_content(data, mimeType);
}

void SCI::BAT::Webserver::HTTPController::RenderView(const httplib::Request& request, httplib::Response& response, const std::filesystem::path& view, const inja::json& data, const std::string& mimeType)
{
    SCI_ASSERT_FMT(m_renderer, "Render on controller \"{}\" ({}) not set", ToString(), typeid(*this).name());
    
    // Adjust data
    inja::json exdata = data;
    exdata["HOST"] = "https://" + request.get_header_value("Host");

    // Final view render
    auto viewPath = view;
    viewPath.replace_extension("jinja");
    response.set_content(m_renderer->RenderTemplate(viewPath, exdata), mimeType);
}
