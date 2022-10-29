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

#include <iostream>
#include <chrono>
#include <sstream>

int main()
{
    // SSL Certificate generate by postbuild event
    httplib::SSLServer svr("./cert.pem", "./key.pem");

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
