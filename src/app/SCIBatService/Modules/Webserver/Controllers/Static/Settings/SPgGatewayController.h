/*!
 * @file SPgGatewayController.h
 * @brief Controller for showing the gateway settings
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

namespace SCI::BAT::Webserver::Controllers
{
    /*!
     * @brief Controller for showing the gateway settings
    */
    class SPgGatewayController : public HTTPController
    {
        public:
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
    };
}
