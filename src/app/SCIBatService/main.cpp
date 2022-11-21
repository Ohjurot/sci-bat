/*
 * 
 * 
 * 
 * 
 */


#include <Threading/ThreadManager.h>
#include <Modules/SCIBatWebserver.h>

#include <SCIUtil/Exception.h>
#include <SCIUtil/KeyboardInterrupt.h>

#include <pugixml.hpp>
#include <argparse/argparse.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>

namespace SCI::BAT
{

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

    int GuardedMain(const argparse::ArgumentParser& args)
    {
        // Extract arguments
        const std::filesystem::path appDirectory = args.get<std::string>("-a");
        const std::filesystem::path confDirectory = args.get<std::string>("-c");

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
        spdlog::info(R"(SSL-Server configuration: "{}:{}" (Cert: "{}", Key: "{}"))", wscServerHost, wscServerPort, (confDirectory / wscSSLCert).generic_string(), (confDirectory / wscSSLKey).generic_string());
        SCI::BAT::SCIBatWebserver webserver(appDirectory / "webserver", wscServerHost, wscServerPort, confDirectory / wscSSLCert, confDirectory / wscSSLKey, CreateLogger(args, "webserver"));
        webserver.RegisterRoutes();

        // Manage the threads
        spdlog::info("Loading ThreadManager");
        SCI::BAT::ThreadManager tmgr;
        tmgr << webserver;

        // Start the thread
        spdlog::info("Target start reached!");
        tmgr.Start();

        // Thread loop
        auto& kbInterrupt = SCI::Util::KeyboardInterrupt::Get();
        kbInterrupt.SetLogger(spdlog::default_logger());
        kbInterrupt.Register();
        while (tmgr())
        {
            if (kbInterrupt.InterupRecived())
            {
                spdlog::info("Stop request received! Terminating modules");
                tmgr.Stop();
            }
            std::this_thread::yield();
        }
        tmgr.Wait();
        spdlog::info("Target stop reached!");

        // Application termination routines


        return 0;
    }
}

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