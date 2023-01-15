/*!
 * @file SysctrlController.h
 * @brief Controller for restarting controlling the system
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <Modules/Gateway/GatewayThread.h>

namespace SCI::BAT::Webserver::Controllers
{
    /*!
     * @brief Controller for restarting controlling the system
    */
    class SysctrlController : public HTTPController
    {
        public:
            void OnPost(const httplib::Request& request, httplib::Response& response) override;
    };
}
