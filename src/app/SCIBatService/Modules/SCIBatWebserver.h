#pragma once

#include <Modules/Webserver/WebserverThread.h>

class MyController : public SCI::BAT::Webserver::HTTPController
{
    public:
        MyController() = delete;

        MyController(std::string str) : m_text(str) {};

        void OnGet(const httplib::Request& request, httplib::Response& response) override
        {
            throw std::logic_error(m_text);
        }

        std::string_view ToString() override
        {
            return m_text;
        }

    private:
        std::string m_text;
};

namespace SCI::BAT
{
    class SCIBatWebserver : public Webserver::WebserverThread
    {
        public:
            using SCI::BAT::Webserver::WebserverThread::WebserverThread;

        private:
            inline void RegisterControllers() override
            {
                RegisterController<MyController>("/a", "Controller A");
                RegisterController<MyController>("/b", "Controller B");
            }

    };
}
