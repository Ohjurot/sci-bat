/*!
 * @file TControlThread.h
 * @brief Builds the temperature contol ontop of a thread
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <Threading/Thread.h>
#include <Config/AuthenticatedConfig.h>
#include <Modules/Mailbox/MailboxThread.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <SCIUtil/Exception.h>

#include <Vendor/serialib.h>
#include <fmt/format.h>
#include <SCIUtil/SPDLogable.h>

#include <string>
#include <chrono>
#include <vector>
#include <filesystem>

namespace SCI::BAT::TControle
{
    /*!
     * @brief Thread implementing temperature control
    */
    class TControlThread : public Thread, public Util::SPDLogable
    {
        public:
            /*!
             * @brief Current operation mode
            */
            enum class OperationMode
            {
                /*! No operation / off */
                Off = 0,
                /*! Cooling */
                Cooling = -1,
                /*! Heating pwr1 (Fan + 1W) */
                HeatingPwr1 = 1,
                /*! Heating pwr1 (Fan + 2W) */
                HeatingPwr2 = 2,
                /*! Heating pwr1 (Fan + 1W + 2W) */
                HeatingPwr3 = 3,
            };

            /*!
             * @brief Current status
            */
            struct Status
            {
                /*! Current operation mode */
                OperationMode mode;
                /*! Status of the relais */
                bool relais[4] = { false, false, false, false };
            };

        public:
            /*!
             * @brief Creates a new parametrized temperature controleer
             * @param mailbox Reference to MQTT mailbox for receiving commands
             * @param logger Logger to be used by thread
            */
            TControlThread(Mailbox::MailboxThread& mailbox, const std::shared_ptr<spdlog::logger>& logger = spdlog::default_logger()) :
                m_mailbox(mailbox)
            {
                SetLogger(logger);
                s_instance = this;
                LoadConfig();
            }

            int ThreadMain() override;

            /*!
             * @brief Retrive the status of the static instance
             * @return Current status 
            */
            static inline Status GetStatus()
            {
                SCI_ASSERT(s_instance, "TControlThread not initialized properly!");
                return { s_instance->m_mode, { s_instance->m_relaisStates[0], s_instance->m_relaisStates[1], s_instance->m_relaisStates[2], s_instance->m_relaisStates[3] } };
            }
            /*!
             * @brief Retrive the thread id of the static instance
             * @return Thread id
            */
            static inline auto GetStaticTID()
            {
                return s_instance->GetTID();
            }
            /*!
             * @brief Retrive the finsihed state of the static instance
             * @return true if finished
            */
            static inline auto IsStaticFinished()
            {
                return s_instance->IsFinished();
            }
            /*!
             * @brief Retrive the current serial device of the static instance
             * @return Name of the current serial device
            */
            static inline auto GetSerialDevice()
            {
                return s_instance->m_serialDevice;
            }
            /*!
             * @brief Retrive if the serial device availability state of the static instance
             * @return True if available
            */
            static inline auto GetDeviceAvailable()
            {
                return s_instance->m_deviceAvailable;
            }
            /*!
             * @brief Retrive if the last command on the serial device was ok on the static instance
             * @return True if last command was ok
            */
            static inline auto GetLastCommandOk()
            {
                return s_instance->m_lastCommandOk;
            }

            /*!
             * @brief Lists all serial device usable on the system
             * @return Vector of devices
            */
            static std::vector<std::string> ListSerialDevices();

        private:
            void LoadConfig();

            bool SetRelais(unsigned int index, bool on);
            bool SerialSend(const void* data, unsigned int byts);

        private:
            static TControlThread* s_instance;
    
            serialib m_serial;

            // User configurable serial
            std::string m_serialDevice = "/dev/tty";
            unsigned int m_fanCooloffTime = 5000;

            // Device available
            bool m_deviceAvailable = false;
            bool m_lastCommandOk = false;

            // Serial interface configuration
            unsigned int m_serialBaude = 9600;
            SerialDataBits m_serialBits = SERIAL_DATABITS_8;
            SerialParity m_serialParity = SERIAL_PARITY_NONE;
            SerialStopBits m_serialStopBits = SERIAL_STOPBITS_1;

            // Operation
            OperationMode m_mode = OperationMode::Off;
            std::chrono::system_clock::time_point m_fanOffTime = std::chrono::system_clock::now();
            bool m_modeApplyed = false;
            bool m_relaisStates[4] = { false, false, false, false };

            // Watchdog
            std::chrono::system_clock::time_point m_watchdogExpires = std::chrono::system_clock::now();
            bool m_watchdogTriped = false;

            // Ref to mailbox
            Mailbox::MailboxThread& m_mailbox;

            // Constant data (controlling the relais)
            const unsigned int m_bytesWordSize = 8;
            const char* const m_bytesOn[4] = {
                "\x55\x56\x00\x00\x00\x01\x01\xAD",
                "\x55\x56\x00\x00\x00\x02\x01\xAE",
                "\x55\x56\x00\x00\x00\x03\x01\xAF",
                "\x55\x56\x00\x00\x00\x04\x01\xB0",
            };
            const char* const m_bytesOff[4] = {
                "\x55\x56\x00\x00\x00\x01\x02\xAE",
                "\x55\x56\x00\x00\x00\x02\x02\xAF",
                "\x55\x56\x00\x00\x00\x03\x02\xB0",
                "\x55\x56\x00\x00\x00\x04\x02\xB1",
            };
    };
}

template <> struct fmt::formatter<SCI::BAT::TControle::TControlThread::OperationMode>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const SCI::BAT::TControle::TControlThread::OperationMode& lhs, FormatContext& ctx) const -> decltype(ctx.out())
    {
        switch (lhs)
        {
            case SCI::BAT::TControle::TControlThread::OperationMode::Off:
                return fmt::format_to(ctx.out(), "Off");
            case SCI::BAT::TControle::TControlThread::OperationMode::Cooling:
                return fmt::format_to(ctx.out(), "Cooling");
            case SCI::BAT::TControle::TControlThread::OperationMode::HeatingPwr1:
                return fmt::format_to(ctx.out(), "HeatingPwr1");
            case SCI::BAT::TControle::TControlThread::OperationMode::HeatingPwr2:
                return fmt::format_to(ctx.out(), "HeatingPwr2");
            case SCI::BAT::TControle::TControlThread::OperationMode::HeatingPwr3:
                return fmt::format_to(ctx.out(), "HeatingPwr3");
            default:
                throw std::runtime_error("Unexpected value");
        }
    }
};
