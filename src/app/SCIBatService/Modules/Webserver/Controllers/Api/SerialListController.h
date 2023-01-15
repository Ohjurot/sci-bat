/*!
 * @file SerialListController.h
 * @brief Controller for listing serial devices
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <Modules/TControle/TControlThread.h>

#include <fmt/format.h>

namespace SCI::BAT::Webserver::Controllers
{
    /*!
     * @brief Controller for listing serial devices
    */
    class SerialListController : public HTTPController
    {
        public:
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
    };
}

