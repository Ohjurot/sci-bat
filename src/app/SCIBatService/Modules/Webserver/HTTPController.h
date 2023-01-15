/*!
 * @file HTTPController.h
 * @brief Base for HTTPControllers
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Config/AuthenticatedConfig.h>
#include <Modules/Webserver/HTTPAuthentication.h>
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

    /*!
     * @brief Base class for HTTPControllers
    */
    class HTTPController : public Util::SPDLogable
    {
        public:
            virtual ~HTTPController() = default;
            
            /*!
             * @brief Converts the controller to string (Text representation)
             * @return Textual information about the controller
            */
            virtual std::string_view ToString() { return ""; };

            /*!
             * @brief Callback that will be called when the server starts listening
             * @param host Host where the server is listening
             * @param port Port where the server is listening
            */
            virtual void OnListen(const std::string_view& host, int port) {};
            /*!
             * @brief Called when the server gets stopped
            */
            virtual void OnStop() {};

            /*!
             * @brief HTTP Get request
             * @param request Input request
             * @param response Output response
            */
            virtual void OnGet(const httplib::Request& request, httplib::Response& response) {};
            /*!
             * @brief HTTP Post request
             * @param request Input request
             * @param response Output response
            */
            virtual void OnPost(const httplib::Request& request, httplib::Response& response) {};
            /*!
             * @brief HTTP Put request
             * @param request Input request
             * @param response Output response
            */
            virtual void OnPut(const httplib::Request& request, httplib::Response& response) {};
            /*!
             * @brief HTTP Patch request
             * @param request Input request
             * @param response Output response
            */
            virtual void OnPatch(const httplib::Request& request, httplib::Response& response) {};
            /*!
             * @brief HTTP Delete request
             * @param request Input request
             * @param response Output response
            */
            virtual void OnDelete(const httplib::Request& request, httplib::Response& response) {};
            /*!
             * @brief HTTP Options request
             * @param request Input request
             * @param response Output response
            */
            virtual void OnOptions(const httplib::Request& request, httplib::Response& response) {};

            /*!
             * @brief Renders the response as MIME type
             * @param response HTTP Response
             * @param data Data as string
             * @param mimeType MIME Type of data
            */
            void RenderMIME(httplib::Response& response, const std::string& data, const std::string& mimeType);
            /*!
             * @brief Renders the response as MIME type
             * @param response HTTP Response
             * @param data Data as pointer
             * @param size Size of data
             * @param mimeType MIME Type of data
            */
            void RenderMIME(httplib::Response& response, const char* data, size_t size, const std::string& mimeType);
            /*!
             * @brief Renders the response from view (template)
             * @param request Input request
             * @param response Output response
             * @param view Path to the view file
             * @param data JSON Data for view rendering
             * @param mimeType Optional MIME customization
            */
            void RenderView(const httplib::Request& request, httplib::Response& response, const std::filesystem::path& view, const inja::json& data = inja::json(), const std::string& mimeType = "text/html;charset=utf-8");

            /*!
             * @brief Renders the response as JSON
             * @param response Output response 
             * @param data JSON Data
            */
            inline void RenderJSON(httplib::Response& response, const inja::json& data)
            {
                RenderMIME(response, data.dump(), "text/json");
            }
            /*!
             * @brief Renders the response from raw HTML
             * @param response Output response  
             * @param html Output HTML
            */
            inline void RenderHTML(httplib::Response& response, const std::string& html)
            {
                RenderMIME(response, html, "text/html;charset=utf-8");
            }

            /*!
             * @brief Sets the response to redirect to a other page
             * @param request Input request
             * @param response Output response
             * @param path Redirection path
            */
            inline void Redirect(const httplib::Request& request, httplib::Response& response, const std::string_view& path)
            {
                std::string url = "https://" + request.get_header_value("Host") + std::string(path);
                response.set_redirect(url);
            }

            /*!
             * @brief Sets the internal HTMLRender object to a certain value
             * @param renderer HTML Renderer to be used by controller
            */
            inline void SetRenderer(HTMLRenderer& renderer)
            {
                m_renderer = &renderer;
            }

            /*!
             * @brief Fills in section information
             * @param data Template rendering data
            */
            inline void Section(inja::json& data)
            {
                data["_S_LEVEL"] = 0;
                data["_SECTION"] = "";
                data["_SUB_SECTION"] = "";
            }
            /*!
             * @brief Fills in section information
             * @param section Name of the section currently displayed
             * @param data Template rendering data
            */
            inline void Section(const std::string& section, inja::json& data)
            {
                data["_S_LEVEL"] = 1;
                data["_SECTION"] = section;
                data["_SUB_SECTION"] = "";
            }
            /*!
             * @brief Fills in section information
             * @param section Name of the section currently displayed
             * @param subSection Name of the sub-section currently displayed
             * @param data Template rendering data
            */
            inline void Section(const std::string& section, const std::string& subSection, inja::json& data)
            {
                data["_S_LEVEL"] = 2;
                data["_SECTION"] = section;
                data["_SUB_SECTION"] = subSection;
            }

            /*!
             * @brief Reads a config string 
             * @param key Config-Key to read from
             * @param user User that is performing the requested config operation
             * @param data JSON Object that shall be overwritten with the configuration on success
             * @return Returns if the operation was successfully
            */
            inline bool ReadConfig(const std::string& key, const HTTPUser& user, nlohmann::json& data)
            {
                return Config::AuthenticateConfig::ReadData(key, (int)user.permissionLevel, data);
            }
            /*!
             * @brief Writes a config string
             * @param key Config-Key to write to
             * @param user User that is performing the requested config operation
             * @param data JSON value to write to the configuration
             * @return Returns if the operation was successfully
            */
            inline bool WriteConfig(const std::string& key, const HTTPUser& user, const nlohmann::json& data)
            {
                return Config::AuthenticateConfig::WriteData(key, (int)user.permissionLevel, data);
            }

            inline std::filesystem::path Path() { return ""; }

        private:
            HTMLRenderer* m_renderer = nullptr;

        public:
            /*!
             * @brief Controller registration function
             * @tparam T Type of the controller to register
             * @tparam ...Args Type of arguments for controller creation
             * @param route Route to register for
             * @param server Server to register on
             * @param registerController Callback to be used on success
             * @param logger SPD Logger for informations
             * @param ...args Arguments for controller creation
             * @return Reference to created controller
            */
            template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<HTTPController, T> && !std::is_same_v<HTTPController, T>&& std::is_constructible_v<T, Args...>>>
            static T& Register(const std::string& route, httplib::Server& server, std::function<void(T*)> registerController, const std::shared_ptr<spdlog::logger>& logger, Args... args)
            {
                // Allocate
                auto* controller = new T(std::forward<Args>(args)...);
                SCI_ASSERT(controller, "Failed to allocate new controller!");

                return Register(controller, route, server, registerController, logger);
            }

        private:
            /*!
             * @brief Internal registration function
             * @tparam T Type of the controller
             * @param controller Pointer to controller
             * @param route Route to register for
             * @param server Server to register on
             * @param registerController Register callback
             * @param logger Logger
             * @return Reference to controller
            */
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

    /*!
     * @brief Helper struct that detects if HTTP Request functions are overridden (Tested on MSVC and G++)
     * @tparam T Controller type
    */
    template<typename T>
    struct HTTPControllerOverloadChecker
    {
        typedef void(*FP)(void);

        /*!
         * @brief Check if the controllers 'OnGet()' function is overloaded.
         * @return True if function 'OnGet()' is overloaded.
        */
        static bool Get()       { return SCI_CASTFP(&T::OnGet)      != SCI_CASTFP(&HTTPController::OnGet);      }
        /*!
         * @brief Check if the controllers 'OnPost()' function is overloaded.
         * @return True if function 'OnPost()' is overloaded.
        */
        static bool Post()      { return SCI_CASTFP(&T::OnPost)     != SCI_CASTFP(&HTTPController::OnPost);     }
        /*!
         * @brief Check if the controllers 'OnPut()' function is overloaded.
         * @return True if function 'OnPut()' is overloaded.
        */
        static bool Put()       { return SCI_CASTFP(&T::OnPut)      != SCI_CASTFP(&HTTPController::OnPut);      }
        /*!
         * @brief Check if the controllers 'OnPatch()' function is overloaded.
         * @return True if function 'OnPatch()' is overloaded.
        */
        static bool Patch()     { return SCI_CASTFP(&T::OnPatch)    != SCI_CASTFP(&HTTPController::OnPatch);    }
        /*!
         * @brief Check if the controllers 'OnDelete()' function is overloaded.
         * @return True if function 'OnDelete()' is overloaded.
        */
        static bool Delete()    { return SCI_CASTFP(&T::OnDelete)   != SCI_CASTFP(&HTTPController::OnDelete);   }
        /*!
         * @brief Check if the controllers 'OnOptions()' function is overloaded.
         * @return True if function 'OnOptions()' is overloaded. 
        */
        static bool Options()   { return SCI_CASTFP(&T::OnOptions)  != SCI_CASTFP(&HTTPController::OnOptions);  }
    };
}
