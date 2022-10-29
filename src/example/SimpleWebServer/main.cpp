/*
 *      Minimal web server example
 *
 *      IMPORTANT: This application depends on multiple files located in "{REPOSITORY}/etc/example-webserver-data/"
 *                 Make sure the application is executed in that directory.
 *
 *      The application will start a HTTPS web server on port 443 (Requires to be executed with proper permission or
 *      sudo on Linux!) 
 *      The following features are provided:
 *      - HTTP GET "/": Root (index) page of the web application will provide a responsive demo page. The demo page 
 *        will dynamicaly display the time when the request was processed by the server.
 *      - HTTP GET "/hi": Will display a plain text "Hello World!" on the screen
 *      - HTTP GET "/res/*": Will dnyamically server the content of the "{REPOSITORY}/etc/example-webserver-data/www-data"
 *        Directory.
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */

#include <httplib.h>
#include <inja/inja.hpp>
#include <argparse/argparse.hpp>
#include <pugixml.hpp>

#include <iostream>
#include <chrono>
#include <sstream>
#include <filesystem>

#ifdef SIMPLEWEBSERVER_WINDOWS
#define SWEB_APP_ROOT "./"
#define SWEB_CONF "./config-win.xml"
#else
#define SWEB_APP_ROOT "/usr/bin/sci-bat-webserver"
#define SWEB_CONF "/etc/sci-bat-example/config.xml"
#endif

int main()
{
    const std::filesystem::path app_root = SWEB_APP_ROOT;
    const std::filesystem::path conf_file = SWEB_CONF;

    // Default config
    std::string host = "localhost";
    int port = 443;
    std::string cert = "./cert.pem";
    std::string key = "./key.pem";

    // Read config
    pugi::xml_document doc;
    if (doc.load_file(conf_file.generic_string().c_str()))
    {
        auto webconf = doc.child("WebServer");
        if (webconf)
        {
            auto att_host = webconf.attribute("Host");
            auto att_port = webconf.attribute("Port");
            auto att_cert = webconf.attribute("SSLCert");
            auto att_key = webconf.attribute("SSLKey");

            host = att_host.as_string(host.c_str());
            port = att_port.as_int(port);
            cert = att_cert.as_string(cert.c_str());
            key = att_key.as_string(key.c_str());
        }
    }

    // SSL Certificate generate by postbuild event
    httplib::SSLServer svr(cert.c_str(), key.c_str());

    // Hook www-data directory
    svr.set_mount_point("/res", (app_root / "www-data").generic_string());

    // Simple hello world
    svr.Get("/hi", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
        });

    // Some static web page
    svr.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
        // Get current time
        std::stringstream ss;
        auto time = std::chrono::system_clock().now();
        auto ttime = std::chrono::system_clock::to_time_t(time);
        ss << std::put_time(std::localtime(&ttime), "%d.%m.%Y %H:%M:%S");

        // Render HTML
        inja::Environment env;
        inja::json data;
        data["time"] = ss.str();
        auto html = env.render_file((app_root / "templates/index.jinja").generic_string(), data);
        
        // Set HTML
        res.set_content(html, "text/html; charset=utf-8");
    });

    // Here we go
    svr.listen(host.c_str(), port);
}
