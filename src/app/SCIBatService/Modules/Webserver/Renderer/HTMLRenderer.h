/*!
 * @file HTMLRenderer.h
 * @brief Provides rendering for templates.
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <SCIUtil/Exception.h>
#include <SCIUtil/SPDLogable.h>

#include <inja/inja.hpp>

#include <string>
#include <string_view>
#include <filesystem>

namespace SCI::BAT::Webserver
{
    /*!
     * @brief Class providing html rendering (views with templates)
    */
    class HTMLRenderer : public Util::SPDLogable
    {
        public:
            HTMLRenderer() = delete;
            /*!
             * @brief Create a new instance of the renderer
             * @param rootDir Root directory where template can be found
            */
            HTMLRenderer(const std::filesystem::path& rootDir);
            HTMLRenderer(const HTMLRenderer&) = delete; 
            HTMLRenderer(HTMLRenderer&&) noexcept = default; 

            HTMLRenderer& operator=(const HTMLRenderer&) = delete;
            HTMLRenderer& operator=(HTMLRenderer&&) noexcept = delete;

            /*!
             * @brief Renders from a template file
             * @param view Path to view to render
             * @param data Data to be used during rendering
             * @return Output HTML string
            */
            std::string RenderTemplate(const std::filesystem::path& view, const inja::json& data = inja::json());
            /*!
             * @brief Renders from a template string
             * @param html Input template string
             * @param data Data to be used during rendering
             * @return Output HTML string
            */
            std::string RenderHTML(const std::string_view& html, const inja::json& data = inja::json());

        private:
            std::filesystem::path m_rootDirectory;
    };
}
