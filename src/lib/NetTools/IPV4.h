#pragma once

#include <fmt/format.h>
#include <scn/scn.h>

#include <ostream>
#include <cstdint>
#include <string>
#include <string_view>

namespace RETI::NetTools
{
    // Port Type
    using Port = uint16_t;

    // IPV4 Address 
    struct IPV4Address
    {
        union
        {
            uint32_t _data;
            uint8_t ip[4];
            struct
            {
                uint8_t ip0, ip1, ip2, ip3;
            };
        };

        std::string ToString() const;
        bool Parse(const std::string_view& text);
    };

    // Endpoint
    struct IPV4Endpoint
    {
        IPV4Address address;
        Port port;

        std::string ToString() const;
        bool Parse(const std::string_view& text);
    };
}

// Stream
std::ostream& operator<<(std::ostream& out, const RETI::NetTools::IPV4Address& addr);
std::ostream& operator<<(std::ostream& out, const RETI::NetTools::IPV4Endpoint& ep);

// Scanning
template <>
struct scn::scanner<RETI::NetTools::IPV4Address> : scn::empty_parser
{
    template <typename Context>
    error scan(RETI::NetTools::IPV4Address& addr, Context& ctx)
    {
        return scn::scan_usertype(ctx, "{}.{}.{}.{}", addr.ip0, addr.ip1, addr.ip2, addr.ip3);
    }
};
template <>
struct scn::scanner<RETI::NetTools::IPV4Endpoint> : scn::empty_parser
{
    template <typename Context>
    error scan(RETI::NetTools::IPV4Endpoint& ep, Context& ctx)
    {
        return scn::scan_usertype(ctx, "{}.{}.{}.{}:{}", ep.address.ip0, ep.address.ip1, ep.address.ip2, ep.address.ip3, ep.port);
    }
};
