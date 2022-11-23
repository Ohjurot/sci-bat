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
            RenderView(request, response, "test");
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
