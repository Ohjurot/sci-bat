#include "IPV4.h"

std::string RETI::NetTools::IPV4Address::ToString() const
{
    return fmt::format("{}.{}.{}.{}", ip0, ip1, ip2, ip3);
}

bool RETI::NetTools::IPV4Address::Parse(const std::string_view& text)
{
    return (bool)scn::scan(text, "{}", *this);
}

std::string RETI::NetTools::IPV4Endpoint::ToString() const
{
    return fmt::format("{}.{}.{}.{}:{}", address.ip0, address.ip1, address.ip2, address.ip3, port);
}

bool RETI::NetTools::IPV4Endpoint::Parse(const std::string_view& text)
{
    return (bool)scn::scan(text, "{}", *this);
}

std::ostream& operator<<(std::ostream& out, const RETI::NetTools::IPV4Address& addr)
{
    out << addr.ToString();
    return out;
}

std::ostream& operator<<(std::ostream& out, const RETI::NetTools::IPV4Endpoint& ep)
{
    out << ep.ToString();
    return out;
}
