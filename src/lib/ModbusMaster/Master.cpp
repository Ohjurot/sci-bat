#include "Master.h"

RETI::Modbus::Slave& RETI::Modbus::Master::SetupSlave(const std::string& name, NetTools::IPV4Endpoint& endpoint)
{
    auto itFind = m_slaves.find(name);
    if (itFind != m_slaves.end())
    {
        GetLogger()->error(R"(Can't insert slave "{}" because it already exists!)", name);
        throw std::runtime_error("Slave name violation");
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
        GetLogger()->error(R"(Can't retrieve slave "{}" because it does not exist!)", name);
        throw std::runtime_error("Slave name violation");
    }
    return m_slaves.at(name);
}

bool RETI::Modbus::Master::IOUpdate()
{
    size_t errorCount = 0;
    for (auto& slave : m_slaves)
    {
        GetLogger()->debug(R"(Updating slave "{}"...)", slave.first);
        auto updateResult = slave.second.ExecuteIOUpdate(m_processImage);
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
        }
    }

    if (errorCount != 0)
    {
        GetLogger()->warn("Slave updates incomplete! Updated {}/{} slaves sucessfully.", m_slaves.size() - errorCount, m_slaves.size());
    }

    return errorCount == 0;
}
