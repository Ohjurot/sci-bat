/*
 *      Base class and helpers for an HTTPController (One route)
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <SCIUtil/Exception.h>

#include <httplib/httplib.h>
#include <spdlog/spdlog.h>

#include <string>
#include <string_view>
#include <type_traits>
#include <functional>

namespace SCI::BAT::Webserver
{
    template<typename T>
    struct HTTPControllerOverloadChecker;

    class HTTPController
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
        static constexpr bool Get() { return &T::OnGet != &HTTPController::OnGet; }
        static constexpr bool Post() { return &T::OnPost != &HTTPController::OnPost; }
        static constexpr bool Put() { return &T::OnPut != &HTTPController::OnPut; }
        static constexpr bool Patch() { return &T::OnPatch != &HTTPController::OnPatch; }
        static constexpr bool Delete() { return &T::OnDelete != &HTTPController::OnDelete; }
        static constexpr bool Options() { return &T::OnOptions != &HTTPController::OnOptions; }
    };
}
