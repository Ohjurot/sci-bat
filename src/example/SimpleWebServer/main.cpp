#include <httplib.h>
#include <inja/inja.hpp>

#include <iostream>
#include <chrono>
#include <sstream>

int main()
{
    // SSL Certificate generate by postbuild event
    httplib::SSLServer svr("./cert.pem-sha256", "./key.pem");

    // Hook www-data directory
    svr.set_mount_point("/res", "./www-data");

    // Simple hello world
    svr.Get("/hi", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
        });

    // Some static web page
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        // Get current time
        std::stringstream ss;
        auto time = std::chrono::system_clock().now();
        auto ttime = std::chrono::system_clock::to_time_t(time);
        ss << std::put_time(std::localtime(&ttime), "%d.%m.%Y %H:%M:%S");

        // Render HTML
        inja::Environment env;
        inja::json data;
        data["time"] = ss.str();
        auto html = env.render_file("./templates/index.jinja", data);
        
        // Set HTML
        res.set_content(html, "text/html; charset=utf-8");
    });

    // Here we go
    svr.listen("localhost", 443);
}
