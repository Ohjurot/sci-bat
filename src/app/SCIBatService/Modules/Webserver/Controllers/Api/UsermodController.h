/*!
 * @file UsermodController.h
 * @brief Controller for modifying users
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <Config/AuthenticatedConfig.h>

#include <fmt/format.h>

namespace SCI::BAT::Webserver::Controllers
{
    /*!
     * @brief Controller for modifying users
    */
    class UsermodController : public HTTPController
    {
        public:
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
            void OnPost(const httplib::Request& request, httplib::Response& response) override;
    };
}
