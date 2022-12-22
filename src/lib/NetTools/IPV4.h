 /*!
  * @file IPV4.h
  * @brief Utility class for IPV4 Addresses and Endpoints (Address + Port).
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma once

#include <fmt/format.h>
#include <scn/scn.h>

#include <ostream>
#include <cstdint>
#include <string>
#include <string_view>

namespace SCI::NetTools
{
    /*!
     * @brief Typedef for TCP/UDP ports.
    */
    using Port = uint16_t;

    /*!
     * @brief IPv4 Address in multiple formats.
    */
    struct IPV4Address
    {
        union
        {
            /*! Raw 32-Bit data. */
            uint32_t _data;
            /*! Data as 4x8-Bit encoded in array. */
            uint8_t ip[4];
            struct
            {
                /*! First 8-Bit of address.  */
                uint8_t ip0;
                /*! Second 8-Bit of address.  */
                uint8_t ip1;
                /*! Third 8-Bit of address.  */
                uint8_t ip2;
                /*! Fourth 8-Bit of address.  */
                uint8_t ip3;
            };
        };

        /*!
         * @brief Converts an IPv4 address to string.
         * @return String representation of address object.
        */
        std::string ToString() const;

        /*!
         * @brief Parses a string into this object.
         * @param text Input string.
         * @return True if parsing succeeded.
        */
        bool Parse(const std::string_view& text);
    };

    /*!
     * @brief IPv4 endpoint. Combination of IPv4 Address and port.
    */
    struct IPV4Endpoint
    {
        /*! IPv4 address. */
        IPV4Address address;
        /*! TCP/UDP port. */
        Port port;

        /*!
         * @brief Converts an IPv4 endpoint to string.
         * @return 
        */
        std::string ToString() const;

        /*!
         * @brief Parses a string into this object.
         * @param text Input string.
         * @return True if parsing succeeded.
        */
        bool Parse(const std::string_view& text);
    };
}

// Stream
std::ostream& operator<<(std::ostream& out, const SCI::NetTools::IPV4Address& addr);
std::ostream& operator<<(std::ostream& out, const SCI::NetTools::IPV4Endpoint& ep);

/*!
 * @brief Input scanner for IPv4 address.
*/
template <>
struct scn::scanner<SCI::NetTools::IPV4Address> : scn::empty_parser
{
    template <typename Context>
    error scan(SCI::NetTools::IPV4Address& addr, Context& ctx)
    {
        return scn::scan_usertype(ctx, "{}.{}.{}.{}", addr.ip0, addr.ip1, addr.ip2, addr.ip3);
    }
};
/*!
 * @brief Input scanner for IPv4 endpoint.
*/
template <>
struct scn::scanner<SCI::NetTools::IPV4Endpoint> : scn::empty_parser
{
    template <typename Context>
    error scan(SCI::NetTools::IPV4Endpoint& ep, Context& ctx)
    {
        return scn::scan_usertype(ctx, "{}.{}.{}.{}:{}", ep.address.ip0, ep.address.ip1, ep.address.ip2, ep.address.ip3, ep.port);
    }
};
