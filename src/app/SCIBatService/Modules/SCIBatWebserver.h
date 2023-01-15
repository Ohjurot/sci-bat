/*!
 * @file SCIBatWebserver.h
 * @brief Webserver customization 
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/WebserverThread.h>
#include <Modules/Webserver/HTTPAuthentication.h>

namespace SCI::BAT
{
    /*!
     * @brief Webserver customization class. 
     * 
     * This class registers all controllers.
    */
    class SCIBatWebserver : public Webserver::WebserverThread
    {
        public:
            using SCI::BAT::Webserver::WebserverThread::WebserverThread;

        private:
            /*!
             * @brief Called when the webserver is starting. Registers all controllers
            */
            void RegisterControllers() override;
    };
}
