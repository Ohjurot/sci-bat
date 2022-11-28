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
    struct HTTPUser
    {
        enum class PermissionLevel : int
        {
            Unauthenticated = -1,
            Viewer = 0,
            Operator = 1,
            Admin = 2,
            SuperAdmin = 10,
            System = 100, // NOT INTEDED TO BE USE AS A USER AUTHENTICATION LEVEL (ONLY THE SYSTEM SHALL ACCESS THIS LEVEL)
        };

        std::string sid = "";
        std::string name = "";
        PermissionLevel permissionLevel = PermissionLevel::Unauthenticated;

        inline bool operator()(PermissionLevel level)
        {
            return permissionLevel == level;
        }
        inline operator bool()
        {
            return permissionLevel != PermissionLevel::Unauthenticated;
        }
    };

    class HTTPAuthentication : public Util::SPDLogable
    {
        private:
            struct SessionData
            {
                std::string sourceAddress;
                std::chrono::system_clock::time_point validUntil;
                std::string userName = "";
                HTTPUser::PermissionLevel permissionLevel = HTTPUser::PermissionLevel::Unauthenticated;
            };

        public:
            static HTTPUser Session(const httplib::Request& request, httplib::Response& response, inja::json& data);
            static HTTPUser Create(const httplib::Request& request, httplib::Response& response, inja::json& data, std::string username, HTTPUser::PermissionLevel permissionLevel);
            static void Destroy(HTTPUser& user);

            static HTTPUser::PermissionLevel GetUserPermission(const HTTPUser& user);

            static std::string HashPassword(const std::string& password);
            static bool CheckPassword(const std::string& storedHash, const std::string& password);

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
