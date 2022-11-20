#pragma once

#include <Threading/Thread.h>

#include <SCIUtil/SPDLogable.h>

#include <httplib/httplib.h>

#include <string>
#include <string_view>
#include <filesystem>

namespace SCI::BAT::Webserver
{
    class WebserverThread : public Thread, public Util::SPDLogable
    {
        public:
            WebserverThread() = delete;
            WebserverThread(const std::filesystem::path& serverRootDir, const std::string_view& host, int port, const std::filesystem::path& certPath, const std::filesystem::path& keyPath);
            WebserverThread(const WebserverThread&) = delete;
            WebserverThread(WebserverThread&&) noexcept = delete;

            ~WebserverThread() = default;

            WebserverThread& operator=(const WebserverThread&) = delete;
            WebserverThread& operator=(WebserverThread&&) = delete;

        protected:
            int ThreadMain() override;
            void OnStop() override;

        private:
            void SetupRoutes(httplib::SSLServer& server);

            void OnRequestLog(const httplib::Request& request, const httplib::Response& response);

        private:
            std::filesystem::path m_rootDirectory;

            httplib::SSLServer m_server;

    };
}
