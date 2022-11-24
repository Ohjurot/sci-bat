/*
 *      Core header compositing the sci-bat webserver
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/WebserverThread.h>

class MyController : public SCI::BAT::Webserver::HTTPController
{ 
    public:
        MyController() = default;

        void OnGet(const httplib::Request& request, httplib::Response& response) override
        {
            inja::json data;
            data["TITLE"] = "Example page";
            data["USERNAME"] = "Test User";
            data["_S_LEVEL"] = 2;
            data["_SECTION"] = "SectionA";
            data["_SUB_SECTION"] = "SubS";
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
            inline void RegisterControllers() override
            {
                RegisterController<MyController>("/");
            }

    };
}
