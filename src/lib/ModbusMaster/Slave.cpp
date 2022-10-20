#include "Slave.h"


RETI::Modbus::Slave::Slave(Slave&& other) noexcept
{
    m_connection = std::move(other.m_connection);
    m_mappings = std::move(other.m_mappings);
    
    m_valid = other.m_valid;
    other.m_valid = false;
}

RETI::Modbus::Slave& RETI::Modbus::Slave::operator=(Slave&& other) noexcept
{
    if (this != &other)
    {
        this->~Slave();
        new(this)Slave(std::move(other));
    }

    return *this;
}

RETI::Modbus::Slave& RETI::Modbus::Slave::Map(const Mapping& mapping)
{
    ValidateMapping(mapping);
    m_mappings.push_back(mapping);

    return *this;
}

RETI::Modbus::Slave& RETI::Modbus::Slave::Map(Mapping&& mapping)
{
    ValidateMapping(mapping);
    m_mappings.push_back(std::move(mapping));

    return *this;
}

void RETI::Modbus::Slave::ValidateMapping(const Mapping& mapping) const
{
    // Check remote size
    if (mapping.Remote.startAddess + mapping.Remote.count < mapping.Remote.startAddess)
    {
        GetLogger()->error(R"(Can't use "mapping.Remote.startAddess" = {} and "mapping.Remote.count" = {}! Out of range!)", mapping.Remote.startAddess, mapping.Remote.count);
        throw std::runtime_error("Mapping validation failed!");
    }
    // Check bits offset
    if (mapping.Local.bitOffset != 0 && !(mapping.Remote.type == RemoteMappingType::DigitalInput || mapping.Remote.type == RemoteMappingType::DigitalOutput))
    {
        GetLogger()->error(R"(A "mapping.Local.bitOffset" of {} (not equal zero) is only valid on binary/digital inputs and outputs!)", mapping.Local.bitOffset);
        throw std::runtime_error("Mapping validation failed!");
    }
    if (mapping.Local.bitOffset > 7)
    {
        GetLogger()->error(R"(A "mapping.Local.bitOffset" of {} (bigger then seven) is invalid!)", mapping.Local.bitOffset);
        throw std::runtime_error("Mapping validation failed!");
    }
    // Check mapping size
    if (mapping.Remote.count > 128)
    {
        GetLogger()->error(R"(Mappings can only be of a maximum of 128 elements!)");
        throw std::runtime_error("Mapping validation failed!");
    }
}

RETI::Modbus::Slave::IOUpdateResult RETI::Modbus::Slave::ExecuteIOUpdate(ProcessImage& processImage)
{
    if (!m_valid)
        return IOUpdateResult::InvalidSlave;

    // Connect to slave
    if (m_connection.Connect())
    {
        // For safety we will create a copy of the process image
        ProcessImage piCopy = processImage;

        // Update all mappings
        size_t errorCount = 0;
        m_connection.Execute([&](RETI::Modbus::MSConnection& c) {
            for (const auto& mapping : m_mappings)
            {
                size_t bitCount = mapping.Remote.count * 8;
                switch (mapping.Remote.type)
                {
                    case RemoteMappingType::AnalogInput:
                        if (!c.ReadAnalogIn(mapping.Remote.startAddess, mapping.Remote.count, (uint16_t*)&piCopy.GetInputBuffer()[mapping.Local.byteOffset]))
                        {
                            errorCount++;
                        }
                        break;
                    case RemoteMappingType::AnalogOutput:
                        if (!c.WriteAnalogOut(mapping.Remote.startAddess, mapping.Remote.count, (uint16_t*)&piCopy.GetOutputBuffer()[mapping.Local.byteOffset]))
                        {
                            errorCount++;
                        }
                        break;
                    case RemoteMappingType::DigitalInput:
                        for (size_t i = 0; i < mapping.Remote.count; i++)
                        {
                            uint8_t bit = (i + mapping.Local.bitOffset) % 8;
                            size_t byte = (i + mapping.Local.bitOffset) / 8;
                            bool value;
                            if (c.ReadDigitalIn(mapping.Remote.startAddess + i, value))
                            {
                                auto& target = piCopy.GetInputBuffer()[byte];
                                target ^= (-(uint8_t)value ^ target) & (1UL << bit);
                            }
                            else
                            {
                                errorCount++;
                            }
                        }
                        break;
                    case RemoteMappingType::DigitalOutput:
                        for (size_t i = 0; i < mapping.Remote.count; i++)
                        {
                            uint8_t bit = (i + mapping.Local.bitOffset) % 8;
                            size_t byte = (i + mapping.Local.bitOffset) / 8;
                            bool value = piCopy.GetOutputBuffer()[byte] & (1UL << bit);
                            if (!c.WriteDigitalOut(mapping.Remote.startAddess + i, value))
                            {
                                errorCount++;
                            }
                        }
                        break;
                }
            }
        });
        m_connection.Disconnect();

        // Evaluate result
        if (errorCount == 0)
        {
            processImage = std::move(piCopy);
            return IOUpdateResult::UpdateSuccess;
        }
        
        GetLogger()->error("Slave ({}) mapping update failed process {}/{} mappings.", m_connection.GetEndpoint().ToString(), m_mappings.size() - errorCount, m_mappings.size());
        return IOUpdateResult::UpdateFailed;
    }
    
    GetLogger()->error("Slave ({}) not reachable!", m_connection.GetEndpoint().ToString());
    return IOUpdateResult::ConnectionError;
}

