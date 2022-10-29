/*
 *      Class that connects to and maps IO from a modbus slave
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <ModbusMaster/MSConnection.h>
#include <ModbusMaster/ProcessImage.h>

#include <SCIUtil/SPDLogable.h>

#include <fmt/format.h>

#include <vector>
#include <string>
#include <sstream>

namespace SCI::Modbus
{
    // Defines a single slave
    class Slave : public Util::SPDLogable
    {
        public:

            // Modbus sided mapping type
            enum class RemoteMappingType
            {
                DigitalInput,
                DigitalOutput,
                AnalogInput,
                AnalogOutput,
            };

            // Maps local and remote registers
            struct Mapping
            {
                struct
                {
                    RemoteMappingType type;
                    uint16_t startAddess;
                    uint16_t count;
                } Remote;
                struct
                {
                    size_t byteOffset;
                    uint8_t bitOffset;
                } Local;
            };

            // Update result
            enum class IOUpdateResult
            {
                InvalidSlave,
                ConnectionError,
                UpdateFailed,
                UpdateSuccess,
                
                FailedConnectionDelay,
                ConnectionStilFailing,
                ConnectionRestoredAndSuccess,
            };

        public:
            Slave() = default;
            Slave(const SCI::NetTools::IPV4Endpoint& endpoint) : 
                m_valid(true), m_connection(endpoint)
            {};
            Slave(const Slave&) = delete;
            Slave(Slave&& other) noexcept;
            
            Slave& operator=(const Slave&) = delete;
            Slave& operator=(Slave&& other) noexcept;

            Slave& Map(const Mapping& mapping);
            Slave& Map(Mapping&& mapping);
            inline Slave& Map(RemoteMappingType remoteType, uint16_t remoteAddress, uint16_t count, size_t localByteAddress, uint8_t localBitAddress = 0)
            {
                return Map({remoteType, remoteAddress, count, localByteAddress, localBitAddress });
            }

            // Writes outputs / reads inputs
            SCI::Modbus::Slave::IOUpdateResult ExecuteIOUpdate(ProcessImage& processImage, float deltaT);
            inline IOUpdateResult operator()(ProcessImage& processImage, float deltaT)
            {
                return ExecuteIOUpdate(processImage, deltaT);
            }

        private:
            void ValidateMapping(const Mapping& mapping) const;

        private:
            bool m_valid = false;
            MSConnection m_connection;
            std::vector<Mapping> m_mappings;

            const uint8_t m_subsequentConnectionTimeoutsThreshold = 5;
            uint8_t m_subsequentConnectionTimeouts = 0;
            const float m_connectionRetryTimerPreset = 5.f;
            float m_connectionRetryTimer = .0f;
    };
}
