#include "ModbusSlave.h"

RETI::Modbus::Slave& RETI::Modbus::Slave::Map(const Mapping& mapping)
{
    auto msg = ValidateMapping(mapping);
    if (!msg.empty())
    {
        throw std::runtime_error(msg);
    }

    m_mappings.push_back(mapping);

    return *this;
}

RETI::Modbus::Slave& RETI::Modbus::Slave::Map(Mapping&& mapping)
{
    auto msg = ValidateMapping(mapping);
    if (!msg.empty())
    {
        throw std::runtime_error(msg);
    }

    m_mappings.push_back(std::move(mapping));

    return *this;
}

std::string RETI::Modbus::Slave::ValidateMapping(const Mapping& mapping)
{
    std::stringstream errorMessage;

    // Check bits offset
    if (mapping.Local.bitOffset != 0 && !(mapping.Remote.type == RemoteMappingType::DigitalInput || mapping.Remote.type == RemoteMappingType::DigitalOutput))
    {
        errorMessage << fmt::format(R"(A "mapping.Local.bitOffset" of {} (not equal zero) is only valid on binary/digital inputs and outputs!)", mapping.Local.bitOffset) << std::endl; 
    }
    if (mapping.Local.bitOffset > 7)
    {
        errorMessage << fmt::format(R"(A "mapping.Local.bitOffset" of {} (bigger then seven) is invalid!)", mapping.Local.bitOffset);
    }
    // Check directions
    if (mapping.Local.type == LocalMappingType::Output && (mapping.Remote.type == RemoteMappingType::AnalogInput || mapping.Remote.type == RemoteMappingType::DigitalInput))
    {
        errorMessage << "Can't map Analog/Digital-Input --> Output!" << std::endl;
    }
    if (mapping.Local.type == LocalMappingType::Input && (mapping.Remote.type == RemoteMappingType::AnalogOutput || mapping.Remote.type == RemoteMappingType::DigitalOutput))
    {
        errorMessage << "Can't map Analog/Digital-Output --> Input!" << std::endl;
    }
    // Check remote size
    if (mapping.Remote.startAddess + mapping.Remote.count < mapping.Remote.startAddess)
    {
        errorMessage << fmt::format(R"(Can't use "mapping.Remote.startAddess" = {} and "mapping.Remote.count" = {}! Out of range!)", mapping.Remote.startAddess, mapping.Remote.count);
    }

    return errorMessage.str();
}
