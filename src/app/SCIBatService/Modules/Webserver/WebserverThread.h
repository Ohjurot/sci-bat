/*!
 * @file WebserverThread.h
 * @brief Builds the webserver ontop of a thread
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */ 
#pragma once

#include <Threading/Thread.h>
#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>
#include <Modules/Webserver/Renderer/HTMLRenderer.h>

#include <SCIUtil/SPDLogable.h>

#include <httplib/httplib.h>

#include <string>
#include <sstream>
#include <vector>
#include <string_view>
#include <filesystem>

namespace SCI::BAT::Webserver
{
    /*!
     * @brief Class providing web server functionality as a separate thread.
    */
    class WebserverThread : public Thread, public Util::SPDLogable
    {
        public:
            WebserverThread() = delete;
            /*!
             * @brief Construct a parameterized web server
             * @param serverRootDir Root directory where the web servers data is stored
             * @param host Server hostname
             * @param port Server port
             * @param certPath Path to the SSL Certificate
             * @param keyPath Path to the SSL Key
             * @param logger SPD logger for the server
             * @param webappLogger SPD logger for the webapp
            */
            WebserverThread(const std::filesystem::path& serverRootDir, const std::string_view& host, int port, const std::filesystem::path& certPath, const std::filesystem::path& keyPath, const std::shared_ptr<spdlog::logger>& logger = spdlog::default_logger(), const std::shared_ptr<spdlog::logger>& webappLogger = spdlog::default_logger());
            WebserverThread(const WebserverThread&) = delete;
            WebserverThread(WebserverThread&&) noexcept = delete;

            virtual ~WebserverThread();

            WebserverThread& operator=(const WebserverThread&) = delete;
            WebserverThread& operator=(WebserverThread&&) = delete;

            /*!
             * @brief Called to register all server routes
            */
            void RegisterRoutes();
            
            /*!
             * @brief Generate the footer used on error pages
             * @return Error footer text 
            */
            inline std::string& ErrorFooter() { return m_finalErrorFooter; }

        protected:
            int ThreadMain() override;
            void OnStop() override;

            /*!
             * @brief Registers a controller on the web server to serve a specific route
             * @tparam T Type of the controller to be registered
             * @tparam ...Args Type of the arguments to be passed on controller creation
             * @param route HTTP Route identification string
             * @param ...args Arguments for the controller
             * @return Reference to create controller
            */
            template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<HTTPController, T> && !std::is_same_v<HTTPController, T> && std::is_constructible_v<T, Args...>>>
            T& RegisterController(const std::string& route, Args... args)
            {
                return HTTPController::Register<T>(route, m_server, std::bind(&WebserverThread::OnControllerAdd, this, std::placeholders::_1), GetLogger(), std::forward<Args>(args)...);
            }

        private:
            /*!
             * @brief Virtual function that can be used when extended to register controllers
            */
            virtual void RegisterControllers() = 0;

            /*!
             * @brief Callback function called when a controller is added
             * @param controller Pointer to controller
            */
            void OnControllerAdd(HTTPController* controller);

            /*!
             * @brief Callback function when a request should be logged
             * @param request Input request
             * @param response Output response
            */
            void OnRequestLog(const httplib::Request& request, const httplib::Response& response);
            /*!
             * @brief Callback function for errors page rendering
             * @param request Input request
             * @param response Output response
            */
            void OnRequestError(const httplib::Request& request, httplib::Response& response);
            /*!
             * @brief Callback function for exceptions occurred during request handling
             * @param request Input request
             * @param response Output response
             * @param pex Exception pointer holding exception information
            */
            void OnRequestException(const httplib::Request& request, httplib::Response& response, std::exception_ptr pex);

            /*!
             * @brief Function that renders a template exception to response
             * @param code HTTP Error code
             * @param description HTTP Error description
             * @param request Input request
             * @param response Output response
            */
            void RenderTemplatedError(int code, const std::string_view& description, const httplib::Request& request, httplib::Response& response);
            /*!
             * @brief Function that renders a final (text based) exception to response
             * @param code HTTP Error code
             * @param description HTTP Error description
             * @param request Input request
             * @param response Output response
            */
            void RenderFinalError(int code, const std::string_view& description, const httplib::Request& request, httplib::Response& response);

        private:
            httplib::SSLServer m_server;
            std::string m_serverHost;
            int m_serverPort;
            std::filesystem::path m_rootDirectory;

            std::vector<HTTPController*> m_controllers;
            std::shared_ptr<spdlog::logger> m_webappLogger;

            HTMLRenderer m_renderer;
            const char* m_finalErrorMessage = R"(<!DOCTYPE html><html><body style="padding: 0; margin: 0; background: LightGray;"><div style="min-height: 4em; padding: 1em; background: rgb(220, 10, 10); "><h2>Error Occured (Code {{code}})</h2></div> <div style="padding: 1em;"><p>A fatal error occured! Please contacte the server admin if you think this is an issue!<br/>Message: {{description}}<br/><br/><b>{{footer}}</b></p></div></body></html>)";
            std::string m_finalErrorFooter = "";
    };
}
