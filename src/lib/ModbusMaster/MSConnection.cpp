#include "MSConnection.h"

SCI::Modbus::MSConnection::MSConnection(const NetTools::IPV4Endpoint& endpoint, int device) :
    m_ctxEndpoint(endpoint), m_device(device)
{
    m_ctx = modbus_new_tcp(m_ctxEndpoint.address.ToString().c_str(), m_ctxEndpoint.port);
    if (m_ctx && device >= 0) modbus_set_slave(m_ctx, device);
}

SCI::Modbus::MSConnection::MSConnection(MSConnection&& other) noexcept
{
    // Copy
    m_ctx = other.m_ctx;
    m_ctxEndpoint = other.m_ctxEndpoint;
    m_connected = other.m_connected;

    // Invalidate
    other.m_ctx = nullptr;
    other.m_connected = false;
}

SCI::Modbus::MSConnection::~MSConnection()
{
    Disconnect();
    if (m_ctx)
    {
        modbus_free(m_ctx);
    }
}

SCI::Modbus::MSConnection& SCI::Modbus::MSConnection::operator=(MSConnection&& other) noexcept
{
    this->~MSConnection();
    new(this)MSConnection(std::move(other));
    return *this;
}

bool SCI::Modbus::MSConnection::Connect()
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

void SCI::Modbus::MSConnection::Disconnect()
{
    if (IsConnected())
    {
        modbus_close(m_ctx);
        m_connected = false;
    }
}

bool SCI::Modbus::MSConnection::Execute(const std::function<void(MSConnection&)>& f)
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

void SCI::Modbus::MSConnection::Update(const NetTools::IPV4Endpoint& endpoint, int device /*= -1*/)
{
    // Close current connection
    bool shouldConnect = m_connected;
    if (shouldConnect)
        Disconnect();

    // Update internals
    m_ctxEndpoint = endpoint;
    m_device = device;

    // Restart connection
    if (shouldConnect)
        Connect();
}
