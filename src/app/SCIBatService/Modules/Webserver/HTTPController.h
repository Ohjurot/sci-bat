/*
 *      Base class and helpers for an HTTPController (One route)
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Modules/Webserver/Renderer/HTMLRenderer.h>

#include <SCIUtil/Exception.h>
#include <SCIUtil/SPDLogable.h>

#include <httplib/httplib.h>
#include <spdlog/spdlog.h>

#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <functional>

#ifdef SCIBATSERVICE_WINDOWS
#define SCI_CASTFP(fp) (fp)
#else
#define SCI_CASTFP(fp) ((void*)(fp))
#endif

namespace SCI::BAT::Webserver
{
    template<typename T>
    struct HTTPControllerOverloadChecker;

    class HTTPController : public Util::SPDLogable
    {
        public:
            virtual ~HTTPController() = default;
            
            virtual std::string_view ToString() { return ""; };

            virtual void OnListen(const std::string_view& host, int port) {};
            virtual void OnStop() {};

            virtual void OnGet(const httplib::Request& request, httplib::Response& response) {};
            virtual void OnPost(const httplib::Request& request, httplib::Response& response) {};
            virtual void OnPut(const httplib::Request& request, httplib::Response& response) {};
            virtual void OnPatch(const httplib::Request& request, httplib::Response& response) {};
            virtual void OnDelete(const httplib::Request& request, httplib::Response& response) {};
            virtual void OnOptions(const httplib::Request& request, httplib::Response& response) {};

            void RenderMIME(httplib::Response& response, const std::string& data, const std::string& mimeType);
            void RenderMIME(httplib::Response& response, const char* data, size_t size, const std::string& mimeType);
            void RenderView(const httplib::Request& request, httplib::Response& response, const std::filesystem::path& view, const inja::json& data = inja::json(), const std::string& mimeType = "text/html;charset=utf-8");

            inline void RenderJSON(httplib::Response& response, const inja::json& data)
            {
                RenderMIME(response, data.dump(), "text/json");
            }
            inline void RenderHTML(httplib::Response& response, const std::string& html)
            {
                RenderMIME(response, html, "text/html;charset=utf-8");
            }

            inline void Redirect(const httplib::Request& request, httplib::Response& response, const std::string_view& path)
            {
                std::string url = "https://" + request.get_header_value("Host") + std::string(path);
                response.set_redirect(url);
            }

            inline void SetRenderer(HTMLRenderer& renderer)
            {
                m_renderer = &renderer;
            }

            inline void Section(inja::json& data)
            {
                data["_S_LEVEL"] = 0;
                data["_SECTION"] = "";
                data["_SUB_SECTION"] = "";
            }
            inline void Section(const std::string& section, inja::json& data)
            {
                data["_S_LEVEL"] = 1;
                data["_SECTION"] = section;
                data["_SUB_SECTION"] = "";
            }
            inline void Section(const std::string& section, const std::string& subSection, inja::json& data)
            {
                data["_S_LEVEL"] = 2;
                data["_SECTION"] = section;
                data["_SUB_SECTION"] = subSection;
            }

            inline std::filesystem::path Path() { return ""; }

        private:
            HTMLRenderer* m_renderer = nullptr;

        public:
            // void
            template<typename T, typename = std::enable_if_t<std::is_base_of_v<HTTPController, T> && !std::is_same_v<HTTPController, T> && std::is_default_constructible_v<T>>>
            static T& Register(const std::string& route, httplib::Server& server, std::function<void(T*)> registerController, const std::shared_ptr<spdlog::logger>& logger)
            {
                // Allocate
                auto* controller = new T();
                SCI_ASSERT(controller, "Failed to allocate new controller!");

                return Register(controller, route, server, registerController, logger);
            }
            // arg
            template<typename T, typename Arg, typename = std::enable_if_t<std::is_base_of_v<HTTPController, T> && !std::is_same_v<HTTPController, T> && std::is_constructible_v<T, Arg>>>
            static T& Register(const std::string& route, httplib::Server& server, std::function<void(T*)> registerController, const std::shared_ptr<spdlog::logger>& logger, Arg arg)
            {
                // Allocate
                auto* controller = new T(arg);
                SCI_ASSERT(controller, "Failed to allocate new controller!");

                return Register(controller, route, server, registerController, logger);
            }
            // args...
            template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<HTTPController, T> && !std::is_same_v<HTTPController, T>&& std::is_constructible_v<T, Args...>>>
            static T& Register(const std::string& route, httplib::Server& server, std::function<void(T*)> registerController, const std::shared_ptr<spdlog::logger>& logger, Args... args)
            {
                // Allocate
                auto* controller = new T(args...);
                SCI_ASSERT(controller, "Failed to allocate new controller!");

                return Register(controller, route, server, registerController, logger);
            }

        private:
            // Internal register function
            template<typename T, typename = std::enable_if_t<std::is_base_of_v<HTTPController, T> && !std::is_same_v<HTTPController, T>>>
            static T& Register(T* controller, const std::string& route, httplib::Server& server, std::function<void(T*)> registerController, const std::shared_ptr<spdlog::logger>& logger)
            {
                // Info
                const std::type_info& tInfo = typeid(T);
                logger->debug("Registering controller {} (\"{}\")", tInfo.name(), controller->ToString());

                // Functions
                size_t registrations = 0;
                if (HTTPControllerOverloadChecker<T>::Get()) { server.Get(route, std::bind(&HTTPController::OnGet, controller, std::placeholders::_1, std::placeholders::_2)); registrations++; }
                if (HTTPControllerOverloadChecker<T>::Post()) { server.Post(route, httplib::Server::Handler(std::bind(&HTTPController::OnPost, controller, std::placeholders::_1, std::placeholders::_2))); registrations++; }
                if (HTTPControllerOverloadChecker<T>::Put()) { server.Put(route, httplib::Server::Handler(std::bind(&HTTPController::OnPut, controller, std::placeholders::_1, std::placeholders::_2))); registrations++; }
                if (HTTPControllerOverloadChecker<T>::Patch()) { server.Patch(route, httplib::Server::Handler(std::bind(&HTTPController::OnPatch, controller, std::placeholders::_1, std::placeholders::_2))); registrations++; }
                if (HTTPControllerOverloadChecker<T>::Delete()) { server.Delete(route, httplib::Server::Handler(std::bind(&HTTPController::OnDelete, controller, std::placeholders::_1, std::placeholders::_2))); registrations++; }
                if (HTTPControllerOverloadChecker<T>::Options()) { server.Options(route, std::bind(&HTTPController::OnOptions, controller, std::placeholders::_1, std::placeholders::_2)); registrations++; }

                // Check
                if (registrations == 0)
                {
                    logger->warn("Controller \"{}\" doest not implement any actions!", tInfo.name());
                }

                // Register
                registerController(controller);
                return *controller;
            }
    };

    template<typename T>
    struct HTTPControllerOverloadChecker
    {
        typedef void(*FP)(void);

        static bool Get()       { return SCI_CASTFP(&T::OnGet)      != SCI_CASTFP(&HTTPController::OnGet);      }
        static bool Post()      { return SCI_CASTFP(&T::OnPost)     != SCI_CASTFP(&HTTPController::OnPost);     }
        static bool Put()       { return SCI_CASTFP(&T::OnPut)      != SCI_CASTFP(&HTTPController::OnPut);      }
        static bool Patch()     { return SCI_CASTFP(&T::OnPatch)    != SCI_CASTFP(&HTTPController::OnPatch);    }
        static bool Delete()    { return SCI_CASTFP(&T::OnDelete)   != SCI_CASTFP(&HTTPController::OnDelete);   }
        static bool Options()   { return SCI_CASTFP(&T::OnOptions)  != SCI_CASTFP(&HTTPController::OnOptions);  }
    };
}
