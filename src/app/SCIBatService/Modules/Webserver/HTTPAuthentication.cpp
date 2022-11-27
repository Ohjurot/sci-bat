#include "HTTPAuthentication.h"

SCI::BAT::Webserver::HTTPAuthentication SCI::BAT::Webserver::HTTPAuthentication::s_instance;

SCI::BAT::Webserver::HTTPUser SCI::BAT::Webserver::HTTPAuthentication::Session(const httplib::Request& request, httplib::Response& response, inja::json& data)
{
    using namespace std::chrono_literals;
    s_instance.DoCleanup();

    s_instance.GetLogger()->trace("Session authentication requested");

    // Quick and dirty session
    HTTPUser user;
    if (request.has_header("Cookie"))
    {
        std::string cookieHeader = (*request.headers.find("Cookie")).second;
        auto eqPos = cookieHeader.find_first_of('=');
        if (eqPos != std::string::npos)
        {
            std::string cookieName = cookieHeader.substr(0, eqPos);
            std::string cookieValue = cookieHeader.substr(eqPos + 1);
            if (cookieName == "SCI_BAT_AUTH")
            {
                s_instance.GetLogger()->trace("Begining session authentication for session id {}", cookieValue);

                Util::LockGuard janitor(s_instance.m_lock); // Begin critical section

                auto itSession = s_instance.m_sessions.find(cookieValue);
                if (itSession != s_instance.m_sessions.end())
                {
                    auto& sessionData = itSession->second;
                    if (sessionData.sourceAddress == request.remote_addr && sessionData.validUntil >= std::chrono::system_clock::now())
                    {
                        sessionData.validUntil = std::chrono::system_clock::now() + 15min; // Re validate
                        user.sid = cookieValue;
                        user.name = sessionData.userName;
                        user.permissionLevel = sessionData.permissionLevel;
                        s_instance.GetLogger()->debug("Session {} authentication for user {} sucessfull.", cookieValue, sessionData.userName);
                    }
                    else
                    {
                        if (sessionData.sourceAddress != request.remote_addr) s_instance.GetLogger()->warn("Potential security risk. {} tried to access session {} owned by {}", request.remote_addr, cookieValue, sessionData.sourceAddress);
                        if(sessionData.validUntil < std::chrono::system_clock::now()) s_instance.GetLogger()->debug("Session {} for user {} expired.", cookieValue, sessionData.userName);

                        s_instance.m_sessions.erase(itSession); // Delete
                    }
                }
                else
                {
                    s_instance.GetLogger()->debug("Invalid session {} requested", cookieValue);
                }
            }
        }
    }

    s_instance.GetLogger()->trace("Session authentication finished");

    // 15 + 1 Minutes valid cookie (REFRESH)
    if(!user.sid.empty()) 
        response.set_header("Set-Cookie", "SCI_BAT_AUTH=" + user.sid + "; Max-Age=960; Secure; SameSite=Strict");

    // Populate json
    data["USERNAME"] = user.name;
    data["AUTH_LEVEL"] = (int)user.permissionLevel;

    return user;
}

void SCI::BAT::Webserver::HTTPAuthentication::Destroy(HTTPUser& user)
{
    s_instance.DoCleanup();

    s_instance.GetLogger()->trace("Begining destruction of session id {}", user.sid);
    if (!user.sid.empty())
    {
        Util::LockGuard janitor(s_instance.m_lock); // Begin critical section
        auto itFind = s_instance.m_sessions.find(user.sid);
        if (itFind != s_instance.m_sessions.end())
        {
            s_instance.m_sessions.erase(itFind);
        }
        janitor.Release(); // End critical section

        s_instance.GetLogger()->debug("Destroyed session {} for user {}", user.sid, user.name);

        // Change user
        user.sid = "";
        user.permissionLevel = HTTPUser::PermissionLevel::Unauthenticated;
    }

    s_instance.GetLogger()->trace("Session destruction finished");
}

SCI::BAT::Webserver::HTTPUser SCI::BAT::Webserver::HTTPAuthentication::Create(const httplib::Request& request, httplib::Response& response, inja::json& data, std::string username, HTTPUser::PermissionLevel permissionLevel)
{
    using namespace std::chrono_literals;
    s_instance.DoCleanup();

    HTTPUser user;
    user.sid = uuids::to_string(uuids::uuid_system_generator{}());
    user.name = username;
    user.permissionLevel = permissionLevel;

    Util::LockGuard janitor(s_instance.m_lock); // Begin critical section
    auto& uid = s_instance.m_sessions[user.sid];
    uid = {
        .sourceAddress = request.remote_addr,
        .validUntil = std::chrono::system_clock::now() + 15min,
        .userName = user.name,
        .permissionLevel = user.permissionLevel,
    };
    janitor.Release(); // End critical section

    // 15 + 1 Minutes valid cookie
    response.set_header("Set-Cookie", "SCI_BAT_AUTH=" + user.sid + "; Max-Age=960; Secure; SameSite=Strict");

    // Populate json
    data["USERNAME"] = user.name;
    data["AUTH_LEVEL"] = (int)user.permissionLevel;

    s_instance.GetLogger()->debug("Created session {} for user {} {}", user.sid, user.name, request.remote_addr);

    return user;
}

void SCI::BAT::Webserver::HTTPAuthentication::DoCleanup()
{
    using namespace std::chrono_literals;

    auto now = std::chrono::system_clock::now();
    if (now - m_lastCleanup > 15min)
    {
        Util::LockGuard janitor(s_instance.m_lock); // Begin critical section
        if (now - m_lastCleanup > 15min)
        {
            GetLogger()->info("Cleanup of invalid sessions initiated");

            m_lastCleanup = now;

            // Find outdated sessions
            std::vector<std::string> invalidSessions;
            for (auto it : m_sessions)
            {
                if (it.second.validUntil < now)
                {
                    invalidSessions.push_back(it.first);
                }
            }

            // Remove outdated sessions
            for (auto it : invalidSessions)
            {
                m_sessions.erase(m_sessions.find(it));
            }

            GetLogger()->info("Succesfully remove {} invalid sessions.", invalidSessions.size());
        }

    }
}

std::string SCI::BAT::Webserver::HTTPAuthentication::HashPassword(const std::string& password)
{
    CHAR hashOut[crypto_pwhash_STRBYTES];
    SCI_ASSERT(crypto_pwhash_str(hashOut, password.c_str(), password.length(), crypto_pwhash_OPSLIMIT_MIN, crypto_pwhash_MEMLIMIT_MIN) == 0, "Failed to hash input password!");
    return hashOut;
}

bool SCI::BAT::Webserver::HTTPAuthentication::CheckPassword(const std::string& storedHash, const std::string& password)
{
    SCI_ASSERT(storedHash.length() <= crypto_pwhash_STRBYTES, "Invalid hash input size");
    CHAR hashIn[crypto_pwhash_STRBYTES] = { 0x0 };
    memcpy(hashIn, storedHash.c_str(), storedHash.length());
    
    return crypto_pwhash_str_verify(hashIn, password.c_str(), password.length()) == 0;
}
