#pragma once

#include <NetTools/IPV4.h>

// Now modbus can be safely included
#include <modbus/modbus.h>
#include <modbus/modbus-tcp.h>

#include <fmt/format.h>

#include <limits>
#include <cerrno>
#include <functional>
#include <type_traits>

namespace SCI::Modbus
{
    // Class that implements a point to point master ---> slave communication
    class MSConnection
    {
        static_assert(sizeof(bool) == 1, "Connection relies on 1 Byte booleans!");

        public:
            MSConnection() = default;
            MSConnection(const NetTools::IPV4Endpoint& endpoint);
            MSConnection(const MSConnection&) = delete;
            MSConnection(MSConnection&& other) noexcept;
            ~MSConnection();

            MSConnection& operator=(const MSConnection&) = delete;
            MSConnection& operator=(MSConnection&& other) noexcept;

            // Connection management
            bool Connect();
            void Disconnect();
            
            // Execute lambda / function
            bool Execute(const std::function<void(MSConnection&)>& f);

            // Simple getters
            inline bool IsValid() const noexcept
            {
                return m_ctx != nullptr;
            }
            inline const SCI::NetTools::IPV4Endpoint& GetEndpoint() const noexcept
            {
                return m_ctxEndpoint;
            }
            inline bool IsConnected() const noexcept 
            {
                return m_connected;
            }
            inline modbus_t* Get() noexcept 
            { 
                return m_ctx; 
            };

            // Boolean operator (check if is connected)
            operator bool() const noexcept
            {
                return IsConnected();
            }

            // Digital IO (Single)
            inline bool WriteDigitalOut(uint16_t index, bool value)
            {
                return WriteDigitalOut(index, 1, &value);
            }
            inline bool ReadDigitalOut(uint16_t index, bool& out)
            {
                return ReadDigitalOut(index, 1, &out);
            }
            inline bool ReadDigitalIn(uint16_t index, bool& in)
            {
                return ReadDigitalIn(index, 1, &in);
            }

            // Analog IO (Single)
            inline bool WriteAnalogOut(uint16_t index, uint16_t value)
            {
                return WriteAnalogOut(index, 1, &value);
            }
            inline bool ReadAnalogOut(uint16_t index, uint16_t& out)
            {
                return ReadAnalogOut(index, 1, &out);
            }
            inline bool ReadAnalogIn(uint16_t index, uint16_t& in)
            {
                return ReadAnalogIn(index, 1, &in);
            }

            // Digital IO (Multiple)
            bool WriteDigitalOut(uint16_t index, uint16_t count, const bool* values)
            {
                return ModbusIOHelper(&modbus_write_bits, index, count, (const uint8_t*)values);
            }
            bool ReadDigitalOut(uint16_t index, uint16_t count, bool* out)
            {
                return ModbusIOHelper(&modbus_read_bits, index, count, (uint8_t*)out);
            }
            bool ReadDigitalIn(uint16_t index, uint16_t count, bool* in)
            {
                return ModbusIOHelper(&modbus_read_input_bits, index, count, (uint8_t*)in);
            }

            // Analog IO (Multiple)
            bool WriteAnalogOut(uint16_t index, uint16_t count, const uint16_t* values)
            {
                return ModbusIOHelper(&modbus_write_registers, index, count, values);
            }
            bool ReadAnalogOut(uint16_t index, uint16_t count, uint16_t* out)
            {
                return ModbusIOHelper(&modbus_read_registers, index, count, out);
            }
            bool ReadAnalogIn(uint16_t index, uint16_t count, uint16_t* in)
            {
                return ModbusIOHelper(&modbus_read_input_registers, index, count, in);
            }

        private:
            // Modbus IO function helper
            template<typename T, typename F, typename = std::enable_if_t<std::is_pointer_v<T> && std::is_invocable_r_v<int, F, modbus_t*, int, int, T>>>
            bool ModbusIOHelper(F func, uint16_t index, uint16_t count, T data)
            {
                bool result = false;
                if (count >= 1 && count <= 128)
                    Execute([&result, func, index, count, data](MSConnection& c)
                        {
                            result = func(c.Get(), (int)index, (int)count, data) != -1;
                        }
                );
                return result;
            }

        private:
            modbus_t* m_ctx = nullptr;
            NetTools::IPV4Endpoint m_ctxEndpoint;
            bool m_connected = false;
    };
}
