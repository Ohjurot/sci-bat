/*
 * 
 * 
 * 
 * 
 */

#include <Modules/Webserver/WebserverThread.h>

#include <Threading/ThreadManager.h>

#include <SCIUtil/KeyboardInterrupt.h>

#include <argparse/argparse.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>

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
    auto sinkConsole = spdlog::stdout_color_mt(name);
    
    // Configure logger
    if (args["-t"] == true)
      sinkConsole->set_level(spdlog::level::trace);
    else if (args["-d"] == true)
        sinkConsole->set_level(spdlog::level::debug);
    sinkConsole->set_pattern("[%d.%m.%Y %H:%M:%S.%e] [%^%l%$] [%t] [%n] %v");

    return sinkConsole;
}

int main(int argc, char** argv)
{
    // Setup command line arguments
    argparse::ArgumentParser args("sci-bat-service", "0.2.0");
    args.add_description("MQTT <--> Modbus Gateway for SMA Inverter");
    args.add_epilog("(c) Copyright 2022 Smart Chaos Integrations, Ludwig Fuechsl\n"
        "(c) Copyright 2022 Munich university for applied science");
    SetupArguments(args);

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

    // Extract arguments
    const std::filesystem::path appDirectory = args.get<std::string>("-a");
    const std::filesystem::path confDirectory = args.get<std::string>("-c");

    // Create logger sinks
    spdlog::set_default_logger(CreateLogger(args, "main"));

    // Create Webserver module
    spdlog::info("Loading Webserver...");
    SCI::BAT::Webserver::WebserverThread webserver(appDirectory / "webserver", "localhost", 443, confDirectory / "cert.pem", confDirectory / "key.pem");
    webserver.SetLogger(CreateLogger(args, "webserver"));
    
    // Manage the threads
    spdlog::info("Loading ThreadManager...");
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
            spdlog::info("Stop request received! Terminating modules...");
            tmgr.Stop();
        }
        std::this_thread::yield();
    }
    tmgr.Wait();
    spdlog::info("Target stop reached!");

    // Application termination routines
    
}
