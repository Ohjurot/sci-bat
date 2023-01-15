/*!
 * @file SettingsController.h
 * @brief Controller for reading and writing settings
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <Modules/Gateway/GatewayThread.h>
#include <Modules/TControle/TControlThread.h>

#include <fmt/format.h>

namespace SCI::BAT::Webserver::Controllers
{
    /*!
     * @brief Controller for reading and writing settings
    */
    class SettingsController : public HTTPController
    {
        public:
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
            void OnPost(const httplib::Request& request, httplib::Response& response) override;
    };
}
