/*!
 * @file SPgAccountsController.h
 * @brief Controller for showing the accounts settings
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

namespace SCI::BAT::Webserver::Controllers
{
    /*!
     * @brief Controller for showing the accounts settings
    */
    class SPgAccountsController : public HTTPController
    {
        public:
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
    };
}
