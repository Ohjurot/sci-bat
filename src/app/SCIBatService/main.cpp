/*!
 * @file main.cpp
 * @brief Service main functions.
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */

#include <Config/UqlJson.h>
#include <Config/AuthenticatedConfig.h>
#include <Threading/ThreadManager.h>

#include <Modules/SCIBatWebserver.h>
#include <Modules/Mailbox/MailboxThread.h>
#include <Modules/Gateway/GatewayThread.h>
#include <Modules/TControle/TControlThread.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <SCIUtil/Exception.h>
#include <SCIUtil/KeyboardInterrupt.h>

#include <Vendor/serialib.h>
#include <pugixml.hpp>
#include <argparse/argparse.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <sodium.h>

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>

namespace SCI::BAT
{
    /*!
     * @brief Sets up the meta information for program arguments.
     * @param args ArgumentParser that shall be configured.
    */
    void SetupArguments(argparse::ArgumentParser& args)
    {
        // Resolve default directory
        std::string sciBatServiceHomeDir = std::filesystem::current_path().generic_string();
        auto* sciServiceEnv = getenv("SCI_BAT_SERVICE_DIR");
        if (sciServiceEnv)
        {
            sciBatServiceHomeDir = sciServiceEnv;
        }

        // Path to service configuration
        args.add_argument<std::string>("-c", "--conf")
            .help("Path to configuration database")
            .required()
            ;

        // Path to the applications root dir
        args.add_argument<std::string>("-a", "--app")
            .help("Path to application directory")
            .default_value<std::string>(sciBatServiceHomeDir.c_str())
            .required()
            ;

        // Log levels
        args.add_argument("-d", "--debug")
            .help("Enables debug outputs (medium logs)")
            .default_value(false)
            .implicit_value(true)
            ;
        args.add_argument("-t", "--trace")
            .help("Enables trace outputs (big logs)")
            .default_value(false)
            .implicit_value(true)
            ;
    }

    /*!
     * @brief Creates an SPDLog-Logger.
     * @param args ArgumentParser that contains configured logger information.
     * @param name Name of the logger.
     * @return SPDLog logger pointer.
    */
    auto CreateLogger(const argparse::ArgumentParser& args, const char* name)
    {
        // Create logger
        auto logger = spdlog::stdout_color_mt(name);

        // Configure logger
        if (args["-t"] == true)
        {
            logger->set_level(spdlog::level::trace);
        }
        else if (args["-d"] == true)
        {
            logger->set_level(spdlog::level::debug);
        }
        logger->set_pattern("[%d.%m.%Y %H:%M:%S.%e] [%^%l%$] [%t] [%n] %v");

        return logger;
    }

    /*!
     * @brief Helper for creating a user in the database.
     * @param usernamen Username of the user to be created. If the user already exists this function will do nothing.
     * @param authlevel Permision level of the user.
     * @param enabled Enable state of the user. If true the user can login.
    */
    void CreateUser(const std::string& usernamen, int authlevel, bool enabled = true)
    {
        Config::AuthenticateConfig::InsertData("user." + usernamen, (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Unauthenticated, (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::SuperAdmin, (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::System,
            {
                { "username", usernamen },
                { "password", SCI::BAT::Webserver::HTTPAuthentication::HashPassword(usernamen) },
                { "authlevel", authlevel },
                { "enabled", enabled },
            }
        );
    }

    /*!
     * @brief Exception save main function.
     * @param args Arguments passed to the application.
     * @return Application return code.
    */
    int GuardedMain(const argparse::ArgumentParser& args)
    {
        // Extract arguments
        const std::filesystem::path appDirectory = args.get<std::string>("-a");
        const std::filesystem::path confDirectory = args.get<std::string>("-c");

        // Init sodium
        auto sodiumrc = sodium_init();
        SCI_ASSERT_FMT(sodiumrc == 0, "sodium_init failed with code {}", sodiumrc);

        // Init mosquitto
        auto mosquittorc = mosquitto_lib_init();
        SCI_ASSERT_FMT(mosquittorc == MOSQ_ERR_SUCCESS, "mosquitto_lib_init failed with code {}", mosquittorc);

        // Init settings db (data.db)
        auto settingDbPath = confDirectory / "data.db";
        spdlog::info("Inisialising systems configuration database {}", settingDbPath.generic_string());
        Config::UqlJson::Get().Init(settingDbPath);

        // Add default configuration (insert will fail when already in there)
        spdlog::info("Configuring default settings");
        CreateUser("superadmin", (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::SuperAdmin);
        CreateUser("admin", (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Admin, false);
        CreateUser("operator", (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Operator, false);
        CreateUser("viewer", (int)SCI::BAT::Webserver::HTTPUser::PermissionLevel::Viewer, false);

        // Create Webserver module
        spdlog::info("Loading Webserver");
        pugi::xml_document webserverConf;
        auto wscFile = confDirectory / "webserver-conf.xml";
        auto wscResult = webserverConf.load_file(wscFile.generic_string().c_str());
        SCI_ASSERT_FMT(wscResult, "Failed to open file: \"{}\"", wscFile.generic_string());
        auto wscNode = webserverConf.child("Webserver");
        SCI_ASSERT_FMT(wscNode, "Failed to find root \"Webserver\" xml node in configuration \"{}\"", wscFile.generic_string());
        std::string wscServerHost = wscNode.attribute("Host").as_string("localhost");
        int wscServerPort = wscNode.attribute("Port").as_int(443);
        std::string wscSSLCert = wscNode.attribute("SSLCert").as_string("");
        std::string wscSSLKey = wscNode.attribute("SSLKey").as_string("");
        SCI_ASSERT(wscServerPort > 0, "Negative port specified!");
        SCI_ASSERT(!wscSSLCert.empty(), "No path to SSL Certificate provided!");
        SCI_ASSERT(!wscSSLCert.empty(), "No path to SSL Certificate Key provided!");
        spdlog::info(R"(SSL-Server configuration: "{}:{}" (Cert: "{}", Key: "{}").)", wscServerHost, wscServerPort, (confDirectory / wscSSLCert).generic_string(), (confDirectory / wscSSLKey).generic_string());
        SCI::BAT::SCIBatWebserver webserver(appDirectory / "webserver", wscServerHost, wscServerPort, confDirectory / wscSSLCert, confDirectory / wscSSLKey, 
            CreateLogger(args, "webserver"), CreateLogger(args, "webapp"));
        webserver.RegisterRoutes();
        webserver.ErrorFooter() = "&copy; Copyright 2022 Smart Chaos Integrations";

        // Create mailbox module
        spdlog::info("Configuring MQTT Mailbox");
        SCI::BAT::Mailbox::MailboxThread mailbox(CreateLogger(args, "mailbox"));

        // Create gateway module
        spdlog::info("Loading Modbus <---> MQTT Gateway");
        SCI::BAT::Gateway::GatewayThread gateway(mailbox, CreateLogger(args, "gateway"));

        // Create temperature control thread
        spdlog::info("Loading TControl (Relays card)");
        SCI::BAT::TControle::TControlThread tcontrol(mailbox, CreateLogger(args, "tcontrol"));

        // Manage the threads
        spdlog::info("Loading ThreadManager");
        SCI::BAT::ThreadManager tmgr;
        tmgr << webserver << mailbox << gateway << tcontrol;

        // Start the thread
        spdlog::info("Target start reached!");
        tmgr.Start();

        // Thread loop
        auto& kbInterrupt = SCI::Util::KeyboardInterrupt::Get();
        kbInterrupt.SetLogger(spdlog::default_logger());
        kbInterrupt.Register();
        while (tmgr())
        {
            if (kbInterrupt.InterrupRecived())
            {
                spdlog::info("Stop request received! Terminating modules");
                tmgr.Stop();
            }
            std::this_thread::yield();
        }
        tmgr.Wait();
        spdlog::info("Target stop reached!");

        // Application termination routines
        mosquitto_lib_cleanup();

        return 0;
    }
}

/*!
 * @brief C++ Main entry point.
 * @param argc Number of cmd arguments.
 * @param argv Array of cmd string arguments.
 * @return Application return code.
*/
int main(int argc, char** argv)
{
    // Setup command line arguments
    argparse::ArgumentParser args("sci-bat-service", "0.2.0");
    args.add_description("MQTT <--> Modbus Gateway for SMA Inverter");
    args.add_epilog("(c) Copyright 2022 Smart Chaos Integrations, Ludwig Fuechsl\n"
        "(c) Copyright 2022 Munich university for applied science");
    SCI::BAT::SetupArguments(args);

    // Parse command line arguments
    try
    {
        args.parse_args(argc, argv);
    }
    catch (std::exception& ex)
    {
        std::cout << ex.what();
        return -1;
    }

    // Create default logger
    spdlog::set_default_logger(SCI::BAT::CreateLogger(args, "main"));

    // Invoke guarded main
    try
    {
        return SCI::BAT::GuardedMain(args);
    }
    catch (std::exception& ex)
    {
        spdlog::critical("Exception in main: {}", ex.what());
    }
    catch (...)
    {
        spdlog::critical("Unknown exception in main.");
    }

    return -1;
}
