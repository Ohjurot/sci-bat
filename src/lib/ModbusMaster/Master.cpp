#include "Master.h"

RETI::Modbus::Slave& RETI::Modbus::Master::SetupSlave(const std::string& name, NetTools::IPV4Endpoint& endpoint)
{
    auto itFind = m_slaves.find(name);
    if (itFind != m_slaves.end())
    {
        throw std::runtime_error("Modbus slave already exists!");
    }

    auto slave = Slave(endpoint);
    slave.SetLogger(GetLogger());
    m_slaves.emplace(name, std::move(slave));

    return m_slaves.at(name);
}

RETI::Modbus::Slave& RETI::Modbus::Master::SetupSlave(const std::string& name)
{
    auto itFind = m_slaves.find(name);
    if (itFind == m_slaves.end())
    {
        throw std::runtime_error("Modbus slave doesn't exist!");
    }
    return m_slaves.at(name);
}

bool RETI::Modbus::Master::IOUpdate(float deltaT)
{
    size_t errorCount = 0;
    GetLogger()->debug("Slave update started.");
    for (auto& slave : m_slaves)
    {
        GetLogger()->debug(R"(Updating slave "{}"...)", slave.first);
        auto updateResult = slave.second.ExecuteIOUpdate(m_processImage, deltaT);
        switch (updateResult)
        {
            case Slave::IOUpdateResult::InvalidSlave:
                GetLogger()->error(R"(Slave "{}" update failed. Slave is not ready/invalid!)", slave.first);
                errorCount++;
                break;
            case Slave::IOUpdateResult::ConnectionError:
                GetLogger()->error(R"(Slave "{}" update failed. Slave is not reachable!)", slave.first);
                errorCount++;
                break;
            case Slave::IOUpdateResult::UpdateFailed:
                GetLogger()->warn(R"(Slave "{}" update finished with errors!)", slave.first);
                errorCount++;
                break;
            case Slave::IOUpdateResult::UpdateSuccess:
                GetLogger()->debug(R"(Slave "{}" update finished successfully!)", slave.first);
                break;
            case Slave::IOUpdateResult::FailedConnectionDelay:
                GetLogger()->debug(R"(Slave "{}" is in connection delay!)", slave.first);
                // errorCount++; Ommit to not spam console
                break;
            case Slave::IOUpdateResult::ConnectionStilFailing:
                GetLogger()->warn(R"(Slave "{}" still not reachable!)", slave.first);
                errorCount++;
                break;
            case Slave::IOUpdateResult::ConnectionRestoredAndSuccess:
                GetLogger()->info(R"(Slave "{}" connection restored!)", slave.first);
                break;
        }
    }

    if (errorCount != 0)
    {
        GetLogger()->warn("Slave updates incomplete! Updated {}/{} slaves sucessfully.", m_slaves.size() - errorCount, m_slaves.size());
    }
    GetLogger()->debug("Slave update finished.");

    return errorCount == 0;
}

RETI::Modbus::IOHandle RETI::Modbus::Master::At(const std::string_view& name)
{
    // TODO: Search alias list

    // Parse normally
    IOType type;
    IOHandle::DataType dtype;
    size_t byteAddress;
    uint8_t bitAddress;
    if (!ParseAddressString(name, type, dtype, byteAddress, bitAddress))
    {
        throw std::runtime_error("Invalid IO Address");
    }

    return At(type, dtype, byteAddress, bitAddress);
}

RETI::Modbus::IOHandle RETI::Modbus::Master::At(IOType type, IOHandle::DataType dtype, size_t byteAddress, uint8_t bitAddress)
{
    return IOHandle(m_processImage, dtype, type == IOType::Input, byteAddress, bitAddress);
}

bool RETI::Modbus::Master::ParseAddressString(const std::string_view& addressString, IOType& type, IOHandle::DataType& dtype, size_t& byteAddress, uint8_t& bitAddress)
{
    // Search by default naming IW 1
    std::string ftype;
    double faddr;
    if (!scn::scan(addressString, "{:s} {:f}", ftype, faddr))
        return false;

    // Now extract all the data that we have 
    byteAddress = (size_t)std::floor(faddr);
    bitAddress = (uint8_t) ((faddr - byteAddress) * 10.);

    // Now check all valid cases
    if (ftype.length() != 1 && ftype.length() != 2)
        return false;

    // First char
    switch (ftype[0])
    {
        case 'I':
        case 'i':
        case 'E':
        case 'e':
            type = IOType::Input;
            break;
        case 'O':
        case 'o':
        case 'A':
        case 'a':
        case 'Q':
        case 'q':
            type = IOType::Output;
            break;
        default:
            return false;
    }

    // Second char
    if (ftype.length() == 2)
    {
        switch (ftype[1])
        {
            case 'B':
            case 'b':
                dtype = IOHandle::DataType::Byte;
                break;
            case 'W':
            case 'w':
                dtype = IOHandle::DataType::Word;
                break;
            case 'D':
            case 'd':
                dtype = IOHandle::DataType::DWord;
                break;
            case 'Q':
            case 'q':
                dtype = IOHandle::DataType::QWord;
                break;
            default:
                return false;
        }
    }
    else
    {
        dtype = IOHandle::DataType::Bit;
    }

    // Validate
    if (bitAddress != 0 && dtype != IOHandle::DataType::Bit)
        return false;

    return true;
}
