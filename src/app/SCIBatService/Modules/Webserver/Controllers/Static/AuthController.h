/*!
 * @file AuthController.h
 * @brief Controller for authenticating user
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/HTTPController.h>
#include <Modules/Webserver/HTTPAuthentication.h>

namespace SCI::BAT::Webserver::Controllers
{
    /*!
     * @brief Controller for authenticating user
    */
    class AuthController : public HTTPController
    {
        public:
            void OnGet(const httplib::Request& request, httplib::Response& response) override;
            void OnPost(const httplib::Request& request, httplib::Response& response) override;
            void OnDelete(const httplib::Request& request, httplib::Response& response) override;
    };
}

