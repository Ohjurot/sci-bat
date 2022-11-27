/*
 *      Core header compositing the sci-bat webserver
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/WebserverThread.h>
#include <Modules/Webserver/HTTPAuthentication.h>

class MyController : public SCI::BAT::Webserver::HTTPController
{ 
    public:
        MyController() = default;

        void OnGet(const httplib::Request& request, httplib::Response& response) override
        {
            inja::json data;
            data["TITLE"] = "Example page";
            
            auto user = SCI::BAT::Webserver::HTTPAuthentication::Session(request, response, data);

            Section(data);
            RenderView(request, response, "test", data);
        }
};

namespace SCI::BAT
{
    class SCIBatWebserver : public Webserver::WebserverThread
    {
        public:
            using SCI::BAT::Webserver::WebserverThread::WebserverThread;

        private:
            void RegisterControllers() override;
    };
}
