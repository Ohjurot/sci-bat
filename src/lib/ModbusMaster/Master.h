#pragma once

#include <ModbusMaster/Slave.h>
#include <ModbusMaster/ProcessImage.h>

#include <NetTools/IPV4.h>
#include <RETIUtil/SPDLogable.h>

#include <fmt/format.h>

#include <unordered_map>
#include <string>

namespace RETI::Modbus
{
    class Master : public Util::SPDLogable
    {
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

            bool IOUpdate();

            inline ProcessImage& GetProcessImage()
            {
                return m_processImage;
            }

        private:
            std::unordered_map<std::string, Slave> m_slaves;
            ProcessImage m_processImage;
    };
}
