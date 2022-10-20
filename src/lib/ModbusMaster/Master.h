#pragma once

#include <ModbusMaster/Slave.h>
#include <ModbusMaster/IOHandle.h>
#include <ModbusMaster/ProcessImage.h>

#include <NetTools/IPV4.h>
#include <RETIUtil/SPDLogable.h>

#include <fmt/format.h>
#include <scn/scn.h>

#include <unordered_map>
#include <string>
#include <string_view>

namespace RETI::Modbus
{
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

            RETI::Modbus::Slave& SetupSlave(const std::string& name, NetTools::IPV4Endpoint& endpoint);
            RETI::Modbus::Slave& SetupSlave(const std::string& name);

            RETI::Modbus::IOHandle At(const std::string_view& name);
            RETI::Modbus::IOHandle At(IOType type, IOHandle::DataType dtype, size_t byteAddress, uint8_t bitAddress);
            inline RETI::Modbus::IOHandle operator[](const std::string_view& name)
            {
                return At(name);
            }

            bool IOUpdate();
            inline ProcessImage& GetProcessImage()
            {
                return m_processImage;
            }

        private:
            static bool ParseAddressString(const std::string_view& addressString, IOType& type, IOHandle::DataType& dtype, size_t& byteAddress, uint8_t& bitAddress);

        private:
            std::unordered_map<std::string, Slave> m_slaves;
            ProcessImage m_processImage;
    };
}
