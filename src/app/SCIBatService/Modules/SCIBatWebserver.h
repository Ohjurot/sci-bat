/*
 *      Core header compositing the sci-bat webserver
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/WebserverThread.h>
#include <Modules/Webserver/HTTPAuthentication.h>

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
