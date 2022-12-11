#pragma once

#include <Threading/Thread.h>
#include <Config/AuthenticatedConfig.h>
#include <Modules/Mailbox/MailboxThread.h>
#include <Modules/Webserver/HTTPAuthentication.h>

#include <Vendor/serialib.h>
#include <SCIUtil/SPDLogable.h>

namespace SCI::BAT::TControle
{
    class TControlThread : public Thread, public Util::SPDLogable
    {
        public:
            TControlThread(Mailbox::MailboxThread& mailbox, const std::shared_ptr<spdlog::logger>& logger = spdlog::default_logger()) :
                m_mailbox(mailbox)
            {
                SetLogger(logger);
            }

            int ThreadMain() override;
            
        private:
            bool SetRelais(unsigned int index, bool on);
            bool SerialSend(const void* data, unsigned int byts);

        private:
            serialib m_serial;

            std::string m_serialDevice = "/dev/ttyS0";
            unsigned int m_serialBaude = 9600;
            SerialDataBits m_serialBits = SERIAL_DATABITS_8;
            SerialParity m_serialParity = SERIAL_PARITY_NONE;
            SerialStopBits m_serialStopBits = SERIAL_STOPBITS_1;

            // Ref to mailbox
            Mailbox::MailboxThread& m_mailbox;

            // Constant data
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
