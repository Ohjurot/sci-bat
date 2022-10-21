#include <httplib.h>
#include <inja/inja.hpp>

#include <iostream>

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
        inja::Environment env;
        inja::json data;
        auto html = env.render_file("./templates/index.jinja", data);
        res.set_content(html, "text/html");
    });

    // Here we go
    svr.listen("localhost", 443);
}
