#pragma once

#include <Threading/Thread.h>
#include <Modules/Webserver/HTTPController.h>

#include <SCIUtil/SPDLogable.h>

#include <httplib/httplib.h>

#include <string>
#include <vector>
#include <string_view>
#include <filesystem>

namespace SCI::BAT::Webserver
{
    class WebserverThread : public Thread, public Util::SPDLogable
    {
        public:
            WebserverThread() = delete;
            WebserverThread(const std::filesystem::path& serverRootDir, const std::string_view& host, int port, const std::filesystem::path& certPath, const std::filesystem::path& keyPath, const std::shared_ptr<spdlog::logger>& logger = spdlog::default_logger());
            WebserverThread(const WebserverThread&) = delete;
            WebserverThread(WebserverThread&&) noexcept = delete;

            virtual ~WebserverThread();

            WebserverThread& operator=(const WebserverThread&) = delete;
            WebserverThread& operator=(WebserverThread&&) = delete;

            void RegisterRoutes();

        protected:
            int ThreadMain() override;
            void OnStop() override;

            template<typename T, typename = std::enable_if_t<std::is_base_of_v<HTTPController, T> && !std::is_same_v<HTTPController, T> && std::is_default_constructible_v<T>>>
            T& RegisterController(const std::string& route)
            {
                return HTTPController::Register<T>(route, m_server, std::bind(&WebserverThread::OnControllerAdd, this, std::placeholders::_1), GetLogger());
            }
            template<typename T, typename Arg, typename = std::enable_if_t<std::is_base_of_v<HTTPController, T> && !std::is_same_v<HTTPController, T> && std::is_constructible_v<T, Arg>>>
            T& RegisterController(const std::string& route, Arg arg)
            {
                return HTTPController::Register<T>(route, m_server, std::bind(&WebserverThread::OnControllerAdd, this, std::placeholders::_1), GetLogger(), arg);
            }
            template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<HTTPController, T> && !std::is_same_v<HTTPController, T> && std::is_constructible_v<T, Args...>>>
            T& RegisterController(const std::string& route, Args... args)
            {
                return HTTPController::Register<T>(route, m_server, std::bind(&WebserverThread::OnControllerAdd, this, std::placeholders::_1), GetLogger(), args...);
            }

        private:
            virtual void RegisterControllers() = 0;

            void OnControllerAdd(HTTPController* controller);

            void OnRequestLog(const httplib::Request& request, const httplib::Response& response);
            void OnRequestError(const httplib::Request& request, httplib::Response& response);
            void OnRequestException(const httplib::Request& request, httplib::Response& response, std::exception_ptr pex);

        private:
            httplib::SSLServer m_server;
            std::string m_serverHost;
            int m_serverPort;
            std::filesystem::path m_rootDirectory;

            std::vector<HTTPController*> m_controllers;
    };
}
