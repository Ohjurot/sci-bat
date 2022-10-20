#pragma once

#include <ModbusMaster/MSConnection.h>

#include <fmt/format.h>

#include <vector>
#include <string>
#include <sstream>

namespace RETI::Modbus
{
    // Local virtual image mapping type
    enum class LocalMappingType
    {
        Input,
        Output,
    };

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
            uint16_t startAddess, count;
        } Remote;
        struct
        {
            LocalMappingType type;
            size_t byteOffset, bitOffset;
        } Local;
    };

    // Defines a single slave
    class Slave
    {
        public:
            Slave() = default;
            Slave(const RETI::NetTools::IPV4Endpoint& endpoint) : 
                m_connection(endpoint)
            {};
            Slave(const Slave&) = delete;
            Slave(Slave&& other) noexcept = default;
            
            Slave& operator=(const Slave&) = delete;
            Slave& operator=(Slave&& other) noexcept = default;

            Slave& Map(const Mapping& mapping);
            Slave& Map(Mapping&& mapping);
            inline Slave& Map(RemoteMappingType remoteType, uint16_t remoteStart, uint16_t count, LocalMappingType localType, size_t localByteOffset, size_t localBitOffset = 0)
            {
                return Map({remoteType, remoteStart, count, localType, localByteOffset, localBitOffset});
            }

        private:
            std::string ValidateMapping(const Mapping& mapping);

        private:
            MSConnection m_connection;
            std::vector<Mapping> m_mappings;
    };
}
