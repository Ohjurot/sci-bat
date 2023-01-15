/*!
 * @file SIndexController.h
 * @brief Controller for showing the settings index page
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

namespace SCI::BAT::Webserver::Controllers
{
    /*!
     * @brief Controller for showing the settings index page
    */
    class SIndexController : public HTTPController
    {
        public:
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
    };
}

