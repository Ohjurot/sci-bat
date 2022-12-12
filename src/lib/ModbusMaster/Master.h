/*
 *      Modbus master that manages the process image and all slaves
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <ModbusMaster/Slave.h>
#include <ModbusMaster/IOHandle.h>
#include <ModbusMaster/ProcessImage.h>

#include <NetTools/IPV4.h>
#include <SCIUtil/SPDLogable.h>

#include <fmt/format.h>
#include <scn/scn.h>

#include <unordered_map>
#include <string>
#include <string_view>

#ifdef SCI_WINDOWS
#define SCI_MODBUS_ENDIAN SCI::Modbus::Endianness::Little
#else
#define SCI_MODBUS_ENDIAN SCI::Modbus::Endianness::Big 
#endif

namespace SCI::Modbus
{
    enum class Endianness
    {
        Little,
        Big,
    };

    class Master : public Util::SPDLogable
    {
        public:
            enum class IOType
            {
                Input,
                Output
            };

        public:
            Master() = default;
            Master(size_t piInputSize, size_t piOutputSize) :
                m_processImage(piInputSize, piOutputSize)
            {};
            Master(const Master&) = delete;
            Master(Master&& other) noexcept = default;

            Master& operator=(const Master&) = delete;
            Master& operator=(Master&& other) noexcept = default;

            SCI::Modbus::Slave& SetupSlave(const std::string& name, NetTools::IPV4Endpoint& endpoint, int slaveId = -1);
            SCI::Modbus::Slave& SetupSlave(const std::string& name);

            inline Master& Alias(const std::string& addr, const std::string& alias)
            {
                m_aliasMapping[alias] = addr;
                return *this;
            }

            SCI::Modbus::IOHandle At(const std::string_view& name);
            SCI::Modbus::IOHandle At(IOType type, IOHandle::DataType dtype, size_t byteAddress, uint8_t bitAddress);
            inline SCI::Modbus::IOHandle operator[](const std::string_view& name)
            {
                return At(name);
            }

            inline void SetSlaveEndianness(Endianness ed)
            {
                m_swapEndian = ed != SCI_MODBUS_ENDIAN;
            }
            inline void SetSwapEndianness(bool swap)
            {
                m_swapEndian = swap;
            }

            bool IOUpdate(float deltaT);
            inline ProcessImage& GetProcessImage()
            {
                return m_processImage;
            }

            inline bool SlaveConnected(const std::string& name)
            {
                auto it = m_slaves.find(name);
                return it != m_slaves.end() ? it->second.GetLastUpdateOk() : false;
            }

        private:
            static bool ParseAddressString(const std::string_view& addressString, IOType& type, IOHandle::DataType& dtype, size_t& byteAddress, uint8_t& bitAddress);

        private:
            std::unordered_map<std::string, std::string> m_aliasMapping;
            std::unordered_map<std::string, Slave> m_slaves;
            ProcessImage m_processImage;

            bool m_swapEndian = false;
    };
}
