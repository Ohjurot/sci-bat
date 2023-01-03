#include "WebserverThread.h"

SCI::BAT::Webserver::WebserverThread::WebserverThread(const std::filesystem::path& serverRootDir, const std::string_view& host, int port, const std::filesystem::path& certPath, const std::filesystem::path& keyPath, const std::shared_ptr<spdlog::logger>& logger, const std::shared_ptr<spdlog::logger>& webappLogger) :
    m_rootDirectory(serverRootDir),
    m_serverHost(host),
    m_serverPort(port),
    m_server(certPath.generic_string().c_str(), keyPath.generic_string().c_str()),
    m_renderer(serverRootDir / "templates"),
    m_webappLogger(webappLogger)
{
    SetLogger(logger);
    m_renderer.SetLogger(GetLogger());
    HTTPAuthentication::Instance().SetLogger(logger);

    // Bind to port
    GetLogger()->info("Binding server to {}:{}", host, port);
    m_server.bind_to_port(std::string(host), port);
}

SCI::BAT::Webserver::WebserverThread::~WebserverThread()
{
    // Cleanup memory
    for (auto* controller : m_controllers)
    {
        delete controller;
    }
}

void SCI::BAT::Webserver::WebserverThread::RegisterRoutes()
{
    // Setup routes
    GetLogger()->info("Registering controllers");
    RegisterControllers();
    GetLogger()->info("Registered {} controllers", m_controllers.size());
    if (m_controllers.empty())
    {
        GetLogger()->error("No controller registered!");
    }
    for (auto* controller : m_controllers)
    {
        controller->SetLogger(m_webappLogger);
        controller->SetRenderer(m_renderer);
    }

    // Handlers
    GetLogger()->info("Binding static configuration");
    m_server.set_logger(std::bind(&WebserverThread::OnRequestLog, this, std::placeholders::_1, std::placeholders::_2));
    m_server.set_error_handler(std::bind(&WebserverThread::OnRequestError, this, std::placeholders::_1, std::placeholders::_2));
    m_server.set_exception_handler(std::bind(&WebserverThread::OnRequestException, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // Setup static www-data
    auto contentDir = m_rootDirectory / "www-data";
    m_server.set_mount_point("/res", contentDir.generic_string());
}

void SCI::BAT::Webserver::WebserverThread::OnRequestLog(const httplib::Request& request, const httplib::Response& response)
{
    GetLogger()->debug("{} request from {} to \"{}\". In-Bytes: {} Out-Bytes: {} ", request.method, request.remote_addr, request.path, request.body.size(), response.body.size());
}

void SCI::BAT::Webserver::WebserverThread::OnRequestError(const httplib::Request& request, httplib::Response& response)
{
    GetLogger()->trace("Beginning HTTP error handler for for \"{}\" from {}", request.path, request.remote_addr);
    bool handledSuccessfully = true;
    try
    {
        GetLogger()->warn("Request from {} resulted in a {} error. [Request: {} {}]", request.remote_addr, response.status, request.method, request.path);

        // Error page
        RenderTemplatedError(response.status, "", request, response);
    }
    catch (std::exception& ex)
    {
        GetLogger()->critical("Exception occured during generation of the error page: {}", ex.what());
        handledSuccessfully = false;
    }
    catch (...)
    {
        GetLogger()->critical("Unknown exception occurred during generation of the error page.");
        handledSuccessfully = false;
    }

    // Final error page
    if (!handledSuccessfully)
    {
        response.status = 500;
        RenderFinalError(response.status, "Error occurred while reporting exception thrown during web request.", request, response);
    }

    GetLogger()->trace("HTTP error handler finished");
}

void SCI::BAT::Webserver::WebserverThread::OnRequestException(const httplib::Request& request, httplib::Response& response, std::exception_ptr pex)
{
    GetLogger()->trace("Beginning HTTP exception handler for \"{}\" from {}", request.path, request.remote_addr);
    // Re throw
    bool handledSuccessfully = true;
    try
    {
        if (pex)
        {
            GetLogger()->trace("Rethrowing exception");
            std::rethrow_exception(pex);
        }
    }
    catch (std::exception& rex)
    {
        // We don't want to generate any further throw beyond this border! It will crash the server if an exception travels above this function!
        try
        {
            GetLogger()->trace("Exception picked up");
            GetLogger()->error("Exception occured during handling of a HTTP request: {} [Request: {} {}]", rex.what(), request.method, request.path);

            // Render exception
            RenderTemplatedError(response.status, rex.what(), request, response);
        }
        catch (std::exception& ex)
        {
            GetLogger()->critical("Exception occured while trying to report a previous exception: {} [Request: {} {}]", ex.what(), request.method, request.path);
            handledSuccessfully = false;
        }
        catch (...)
        {
            GetLogger()->critical("Unknown exception occurred while trying to report a previous exception! [Request: {} {}]", request.method, request.path);
            handledSuccessfully = false;
        }
    }

    // Handle most fatal error
    if (!handledSuccessfully)
    {
        GetLogger()->trace("Exception handler faulty. Providing bare minimum response.");
        response.status = 500;
        RenderFinalError(response.status, "Error occurred while reporting exception thrown during web request.", request, response);
    }

    GetLogger()->trace("HTTP exception handler finished");
}

int SCI::BAT::Webserver::WebserverThread::ThreadMain()
{
    // Notify controllers
    GetLogger()->info("Starting webserver");
    for (auto* controller : m_controllers)
    {
        controller->OnListen(m_serverHost, m_serverPort);
    }

    // Run server
    GetLogger()->info("Target start reached");
    return m_server.listen_after_bind() ? 0 : -1;
}

void SCI::BAT::Webserver::WebserverThread::OnStop()
{
    // Stop server
    GetLogger()->info("Stopping webserver");
    m_server.stop();

    // Notify controllers
    GetLogger()->info("Propagating stop to controllers");
    for (auto* controller : m_controllers)
    {
        controller->OnStop();
    }

    GetLogger()->info("Reached target stop");
}

void SCI::BAT::Webserver::WebserverThread::OnControllerAdd(HTTPController* controller)
{
    m_controllers.push_back(controller);
}

void SCI::BAT::Webserver::WebserverThread::RenderTemplatedError(int code, const std::string_view& description, const httplib::Request& request, httplib::Response& response)
{
    inja::json data;
    data["code"] = response.status;
    data["description"] = description;
    data["footer"] = m_finalErrorFooter;

    // Required for basic template
    data["HOST"] = "https://" + request.get_header_value("Host");
    data["USERNAME"] = "";
    data["TITLE"] = "Error";
    data["_S_LEVEL"] = 0;

    response.set_content(m_renderer.RenderTemplate("httperror.jinja", data), "text/html;charset=utf-8");
}

void SCI::BAT::Webserver::WebserverThread::RenderFinalError(int code, const std::string_view& description, const httplib::Request& request, httplib::Response& response)
{
    GetLogger()->trace("Error handler faulty. Providing bare minimum response.");
    inja::json data;
    data["code"] = response.status;
    #ifdef _DEBUG
    std::stringstream ss;
    ss << description << "<br/><br/>Requested: " << request.method << " " << request.path << "<br/>";
    for (auto& p : request.params) ss << "Request-Parameter \"" << p.first << "\": \"" << p.second << "\"<br/>";
    for (auto& h : request.headers) ss << "Request-Header \"" << h.first << "\": \"" << h.second << "\"<br/>";
    data["description"] = ss.str();
    #else
    data["description"] = description;
    #endif
    data["footer"] = m_finalErrorFooter;
    response.set_content(m_renderer.RenderHTML(m_finalErrorMessage, data), "text/html;charset=utf-8");
}
