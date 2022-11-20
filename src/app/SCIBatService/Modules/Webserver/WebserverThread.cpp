#include "WebserverThread.h"

SCI::BAT::Webserver::WebserverThread::WebserverThread(const std::filesystem::path& serverRootDir, const std::string_view& host, int port, const std::filesystem::path& certPath, const std::filesystem::path& keyPath) :
    m_rootDirectory(serverRootDir),
    m_server(certPath.generic_string().c_str(), keyPath.generic_string().c_str())
{
    // Setup routes
    SetupRoutes(m_server);

    // Handlers
    m_server.set_logger(std::bind(&WebserverThread::OnRequestLog, this, std::placeholders::_1, std::placeholders::_2));

    // Setup static www-data
    auto contentDir = m_rootDirectory / "www-data";
    m_server.set_mount_point("/res", contentDir.generic_string());

    // Bind to port
    m_server.bind_to_port(std::string(host), port);
}

void SCI::BAT::Webserver::WebserverThread::SetupRoutes(httplib::SSLServer& server)
{

}

void SCI::BAT::Webserver::WebserverThread::OnRequestLog(const httplib::Request& request, const httplib::Response& response)
{
    GetLogger()->debug("{} request from {} to \"{}\". In-Bytes: {} Out-Bytes: {} ", request.method, request.remote_addr, request.path, request.content_length_, response.content_length_);
}

int SCI::BAT::Webserver::WebserverThread::ThreadMain()
{
    // Run server
    GetLogger()->info("Starting webserver");
    return m_server.listen_after_bind() ? 0 : -1;
}

void SCI::BAT::Webserver::WebserverThread::OnStop()
{
    GetLogger()->info("Stopping webserver");
    m_server.stop();
}
