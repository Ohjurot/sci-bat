#include "MSConnection.h"

RETI::Modbus::MSConnection::MSConnection(const NetTools::IPV4Endpoint& endpoint) :
    m_ctxEndpoint(endpoint)
{
    m_ctx = modbus_new_tcp(m_ctxEndpoint.address.ToString().c_str(), m_ctxEndpoint.port);
}

RETI::Modbus::MSConnection::MSConnection(MSConnection&& other) noexcept
{
    // Copy
    m_ctx = other.m_ctx;
    m_ctxEndpoint = other.m_ctxEndpoint;
    m_connected = other.m_connected;

    // Invalidate
    other.m_ctx = nullptr;
    other.m_connected = false;
}

RETI::Modbus::MSConnection::~MSConnection()
{
    Disconnect();
    if (m_ctx)
    {
        modbus_free(m_ctx);
    }
}

RETI::Modbus::MSConnection& RETI::Modbus::MSConnection::operator=(MSConnection&& other) noexcept
{
    this->~MSConnection();
    new(this)MSConnection(std::move(other));
    return *this;
}

bool RETI::Modbus::MSConnection::Connect()
{
    // Disconnect old
    if (IsConnected())
    {
        Disconnect();
    }

    // Connect new
    if (m_ctx)
    {
        m_connected = modbus_connect(m_ctx) != -1;
    }
    return IsConnected();
}

void RETI::Modbus::MSConnection::Disconnect()
{
    if (IsConnected())
    {
        modbus_close(m_ctx);
        m_connected = false;
    }
}

bool RETI::Modbus::MSConnection::Execute(const std::function<void(MSConnection&)>& f)
{
    bool isKeepAlive = IsConnected();
    if (isKeepAlive || Connect())
    {
        f(*this);
        if (!isKeepAlive) 
        {
            Disconnect();
        }

        return true;
    }

    return false;
}
