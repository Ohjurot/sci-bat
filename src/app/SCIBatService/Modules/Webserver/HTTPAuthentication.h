/*!
 * @file HTTPAuthentication.h
 * @brief Provided HTTP Session based Authentication using cookies.
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Config/AuthenticatedConfig.h>

#include <SCIUtil/SPDLogable.h>
#include <SCIUtil/Exception.h>
#include <SCIUtil/Concurrent/SpinLock.h>
#include <SCIUtil/Concurrent/LockGuard.h>

#include <httplib/httplib.h>
#include <inja/inja.hpp>
#include <sodium.h>

#undef __cpp_lib_span
#define UUID_SYSTEM_GENERATOR
#include <uuid.h>
#define __cpp_lib_span

#include <chrono>
#include <string>
#include <vector>
#include <map>

namespace SCI::BAT::Webserver
{
    /*!
     * @brief Information about a user
    */
    struct HTTPUser
    {
        /*!
         * @brief Permission levels a user can have
        */
        enum class PermissionLevel : int
        {
            /*! Not authenticated */
            Unauthenticated = -1,
            /*! Authenticated as viewer */
            Viewer = 0,
            /*! Authenticated as operator */
            Operator = 1,
            /*! Authenticated as admin */
            Admin = 2,
            /*! Authenticated as superadmin */
            SuperAdmin = 10,
            /*! Authenticated as system (NOT INTEDED TO BE USE AS A USER AUTHENTICATION LEVEL! ONLY THE SYSTEM SHALL ACCESS THIS LEVEL) */
            System = 100, 
        };

        /*!
         * @brief Session id of the user
        */
        std::string sid = "";
        /*!
         * @brief Name of the user
        */
        std::string name = "";
        /*!
         * @brief Permission level of the user
        */
        PermissionLevel permissionLevel = PermissionLevel::Unauthenticated;

        /*!
         * @brief Checking if the user has a certain permission level (or higher)
         * @param level Level to check against
         * @return True if users level is bigger or equal than level
        */
        inline bool operator()(PermissionLevel level)
        {
            return (int)permissionLevel >= (int)level;
        }
        /*!
         * @brief Checking is the user is authenticated
        */
        inline operator bool()
        {
            return permissionLevel != PermissionLevel::Unauthenticated;
        }
    };

    /*!
     * @brief Class that provided HTTP session based authentication
    */
    class HTTPAuthentication : public Util::SPDLogable
    {
        private:
            /*!
             * @brief struct containing the data of a session
            */
            struct SessionData
            {
                /*!
                 * @brief Session source address (for validation)
                */
                std::string sourceAddress;
                /*!
                 * @brief Information when the session expires
                */
                std::chrono::system_clock::time_point validUntil;
                /*!
                 * @brief Name of the user using the session
                */
                std::string userName = "";
                /*!
                 * @brief Permission level of the session
                */
                HTTPUser::PermissionLevel permissionLevel = HTTPUser::PermissionLevel::Unauthenticated;
            };

        public:
            /*!
             * @brief Tries to open a existing session
             * @param request Input request
             * @param response Output response
             * @param data HTTP view data
             * @return HTTPUser deduced from session
            */
            static HTTPUser Session(const httplib::Request& request, httplib::Response& response, inja::json& data);
            /*!
             * @brief Creates a new session
             * @param request Input request
             * @param response Output response
             * @param data HTTP view data
             * @param username Name of the user to create session for
             * @param permissionLevel Level of user permission
             * @return HTTPUser created by session
            */
            static HTTPUser Create(const httplib::Request& request, httplib::Response& response, inja::json& data, std::string username, HTTPUser::PermissionLevel permissionLevel);
            /*!
             * @brief Destroys a session for a user
             * @param user User to destroy session for
            */
            static void Destroy(HTTPUser& user);

            /*!
             * @brief Retrive user permission based on its name
             * @param user User to retrieve permission level for
             * @retuen users permission level
            */
            static HTTPUser::PermissionLevel GetUserPermission(const HTTPUser& user);

            /*!
             * @brief Converts a clear text password into an hash
             * @param password Input password
             * @return Output hash
            */
            static std::string HashPassword(const std::string& password);
            /*!
             * @brief Checks if a password matches the stored hash
             * @param storedHash Input stored hash
             * @param password Input clear text password
             * @return true if password matches the hash
            */
            static bool CheckPassword(const std::string& storedHash, const std::string& password);

            /*!
             * @brief Get the instance of the singleton instance
             * @return Reference to instance
            */
            static inline HTTPAuthentication& Instance()
            {
                return s_instance;
            }

        private:
            HTTPAuthentication() = default;
            HTTPAuthentication(const HTTPAuthentication&) = delete;
            HTTPAuthentication& operator=(const HTTPAuthentication&) = delete;

            static HTTPAuthentication s_instance;

            void DoCleanup();

        private:
            Util::SpinLock m_lock;
            std::map<std::string, SessionData> m_sessions;

            std::chrono::system_clock::time_point m_lastCleanup = std::chrono::system_clock::now();
    };
}
