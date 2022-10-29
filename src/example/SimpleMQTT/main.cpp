/*
 *      MQTT Subscribe and Publish Example 
 *
 *      IMPORTANT: This application requires a "settings.xml" file in its working directory. 
 *                 An example configuration can be found here: "{REPOSITORY}/etc/example-mqtt-data/settings.xml"
 * 
 *      The application will connect to the MQTT broker specified in settings.xml it will periodically publish
 *      MQTT data on the topic "example" with a prefix specified in settings.xml. The application subscribes 
 *      to all topics matching the prefix defined in the settings.xml file. 
 *      When the MQTT connection works properly it should echo out all the data that it has sent as string
 *      ("Hello MQTT!")
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */

#include <SCIUtil/KeyboardInterrupt.h>

#include <pugixml.hpp>
#include <spdlog/spdlog.h>
#include <mosquittopp.h>

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>

class MyMQTTClient : public mosqpp::mosquittopp
{
    public:
        MyMQTTClient() : mosqpp::mosquittopp("RETI-TestClient") {}

        void on_publish(int mid) override
        {
            spdlog::info("Message {} Published", mid);
        }


        void on_subscribe(int mid, int rqos, const int* gqos) override
        {
            spdlog::info("Successfully subscribed to a topic!");
        }


        void on_unsubscribe(int mid) override
        {
            spdlog::info("Unsubscribed to message!");
        }

        void on_error() override
        {
            spdlog::error("An mosquitto error occurred!");
        }

        void on_connect(int rc) override
        {
            if (!rc)
            {
                spdlog::info("Connected to MQTT broker");
            }
        }

        void on_message(const mosquitto_message* msg) override
        {
            spdlog::info(R"(Retrive message on topic "{}" with the value "{}")", msg->topic, (const char*)msg->payload, msg->mid);
        }
};

int main()
{
    using namespace std::chrono_literals;

    // Load settings
    pugi::xml_document settings;
    #ifdef SIMPLEMQTT_WINDOWS
    auto settings_load_result = settings.load_file("./settings.xml");
    #else
    auto settings_load_result = settings.load_file("/etc/sci-bat-example-mqtt/settings.xml");
    #endif
    if (!settings_load_result)
    {
        spdlog::error(R"(Failed to open "settings.xml")");
        return -1;
    }

    // Seek root configuration node
    auto settings_node = settings.child("Settings");
    if (!settings_node)
    {
        spdlog::error(R"(Failed to retrieve "Settings" xml node.)");
        return -1;
    }
    auto mqtt_node = settings_node.child("MQTT");
    if (!mqtt_node)
    {
        spdlog::error(R"(Failed to retrieve "Settings/MQTT" xml node.)");
        return -1;
    }

    // Get all attributes
    auto host_att = mqtt_node.attribute("host");
    auto port_att = mqtt_node.attribute("port");
    auto topic_prefix_att = mqtt_node.attribute("topic_prefix");

    // Extract settings
    std::string setting_host = host_att.as_string("");
    uint32_t setting_port = port_att.as_uint(0);
    std::string setting_topic_prefix = topic_prefix_att.as_string("");

    // Validate settings
    if (setting_topic_prefix.empty())
    {
        spdlog::error(R"(An empty topic prefix is not allowed. We need to separate this from all the junk coming into the "test.mosquitto.org" server.)");
        return -1;
    }
    if (setting_host.empty())
    {
        spdlog::error(R"(An empty MQTT host is not allowed!)");
        return -1;
    }
    if (setting_port == 0)
    {
        spdlog::error(R"(Please provide a MQTT broker port)");
        return -1;
    }

    // Init mqtt
    if (mosqpp::lib_init() != MOSQ_ERR_SUCCESS)
    {
        spdlog::error("Failed to init mosquitto lib!");
        return -1;
    }

    // Test
    MyMQTTClient mq;
    int res;
    spdlog::info(R"(Connecting to "{}:{}")", setting_host, setting_port);
    if ((res = mq.connect(setting_host.c_str(), setting_port)) != MOSQ_ERR_SUCCESS) // Unauthenticated and unencrypted
    {
        spdlog::error(R"(Unable to connect to MQTT broker!)");
        return -1;
    }

    // Subscribe
    std::stringstream ss;
    ss << setting_topic_prefix << "/" << "#";
    auto stopic = ss.str();
    int sub_mid = 0;
    spdlog::info(R"(Subscribing to "{}")", stopic);
    mq.subscribe(&sub_mid, stopic.c_str(), 0);

    // KB Interrupt
    auto& kbInterupt = SCI::Util::KeyboardInterrupt::Get();
    kbInterupt.Register();

    // Big loop
    int counter = 0;
    while (!kbInterupt.InterupRecived())
    {
        // Send a message
        if (counter++ == 50)
        {
            counter = 0;
            ss.str("");
            ss << setting_topic_prefix << "/" << "example";
            const char* payload = "Hello MQTT!";
            int pub_mid = 0;
            mq.publish(&pub_mid, ss.str().c_str(), strlen(payload) + 1, payload);
        }

        // Loop
        auto rc = mq.loop(50);
        if (rc)
        {
            mq.reconnect();
        }
    }

    // Unsub
    int usub_mid = 0;
    spdlog::info("Unsubscribing and shutting down");
    mq.unsubscribe(&usub_mid, stopic.c_str());
    mq.disconnect();

    mosqpp::lib_cleanup();
}
