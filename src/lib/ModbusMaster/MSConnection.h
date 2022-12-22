 /*!
  * @file MSConnection.h
  * @brief Modbus TCP master to slave connection
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
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
    /*!
     * @brief Class that implements a point to point master ---> slave modbus communication.
    */
    class MSConnection
    {
        static_assert(sizeof(bool) == 1, "Connection relies on 1 Byte booleans!");

        public:
            MSConnection() = default;
            /*!
             * @brief Creates a new slave with connection information.
             * @param endpoint TCP/IP Endpoint of slave. This will be used to connect to the slave.
             * @param device Slave device id. Set to -1 if not used.
            */
            MSConnection(const NetTools::IPV4Endpoint& endpoint, int device = -1);
            MSConnection(const MSConnection&) = delete;
            MSConnection(MSConnection&& other) noexcept;
            ~MSConnection();

            MSConnection& operator=(const MSConnection&) = delete;
            MSConnection& operator=(MSConnection&& other) noexcept;

            /*!
             * @brief Updates the connection details of the slave.
             * @param endpoint New TCP/IP endpoint to be used.
             * @param device New device id to be used. Use -1 to disable the device id.
            */
            void Update(const NetTools::IPV4Endpoint& endpoint, int device = -1);

            /*!
             * @brief Opens connection to the slave.
             * @return true if connection was successfully.
            */
            bool Connect();

            /*!
             * @brief Disconnects from slave.
            */
            void Disconnect();
            
            /*!
             * @brief Connects to the slave and if successfully executes the function/lambda.
             * 
             * Once the function / lambda finishes the connection state (connected / disconnected) prior to this function call will be restored!
             * @param f Function / lambda to execute.
             * @return True if function / lambda was executed.
            */
            bool Execute(const std::function<void(MSConnection&)>& f);

            /*!
             * @brief Checks if the internal state of the object is valid.
             * @return True if the object is valid.
            */
            inline bool IsValid() const noexcept
            {
                return m_ctx != nullptr;
            }
            /*!
             * @brief Retrieves the current configured endpoint.
             * @return IPv4 endpoint (TCP).
            */
            inline const SCI::NetTools::IPV4Endpoint& GetEndpoint() const noexcept
            {
                return m_ctxEndpoint;
            }
            /*!
             * @brief Retrieves the current connection state of the slave.
             * @return True if currently connected. 
            */
            inline bool IsConnected() const noexcept 
            {
                return m_connected;
            }
            /*!
             * @brief Access to the modbus object
             * @return modbus object 
            */
            inline modbus_t* Get() noexcept 
            { 
                return m_ctx; 
            };

            /*!
             * @brief Boolean operator. Will convert to true if currently connected.
            */
            operator bool() const noexcept
            {
                return IsConnected();
            }

            /*!
             * @brief Writes a single digital output (bit).
             * @param index Output address of modbus slave.
             * @param value State of the output to write.
             * @return True if the call succeeded.
            */
            inline bool WriteDigitalOut(int index, bool value)
            {
                return WriteDigitalOut(index, 1, &value);
            }
            /*!
             * @brief Reads a single digital output (bit).
             * @param index Output address of modbus slave.
             * @param value Reference to a variable that shall match the current output state.
             * @return True if the call succeeded.
            */
            inline bool ReadDigitalOut(int index, bool& out)
            {
                return ReadDigitalOut(index, 1, &out);
            }
            /*!
             * @brief Reads a single digital input (bit).
             * @param index Input address of modbus slave.
             * @param value Reference to a variable that shall match the current input state.
             * @return True if the call succeeded.
            */
            inline bool ReadDigitalIn(int index, bool& in)
            {
                return ReadDigitalIn(index, 1, &in);
            }

            /*!
             * @brief Writes a single analog output value (Holding register).
             * @param index Index of the analog output register.
             * @param value Value that should be written
             * @return True if the call succeeded.
            */
            inline bool WriteAnalogOut(int index, uint16_t value)
            {
                return WriteAnalogOut(index, 1, &value);
            }
            /*!
             * @brief Reads a single analog output value (Holding register).
             * @param index Index of the analog output register.
             * @param out Reference to uint16_t. Will be set to the current state of the output.
             * @return True if the call succeeded.
            */
            inline bool ReadAnalogOut(int index, uint16_t& out)
            {
                return ReadAnalogOut(index, 1, &out);
            }
            /*!
             * @brief Reads a single analog input value (Holding register).
             * @param index Index of the analog input register.
             * @param out Reference to uint16_t. Will be set to the current state of the input.
             * @return True if the call succeeded.
            */
            inline bool ReadAnalogIn(int index, uint16_t& in)
            {
                return ReadAnalogIn(index, 1, &in);
            }

            /*!
              * @brief Writes multiple digital outputs (bit).
              * @param index Output start address of modbus slave.
              * @param count Number of registers to write
              * @param value State of the outputs to write.
              * @return True if the call succeeded.
             */
            bool WriteDigitalOut(int index, uint16_t count, const bool* values)
            {
                return ModbusIOHelper(&modbus_write_bits, index, count, (const uint8_t*)values);
            }
            /*!
             * @brief Reads multiple digital inputs (bit).
             * @param index Input start address of modbus slave.
             * @param count Number of registers to read
             * @param value Array that should be set to the current output values.
             * @return True if the call succeeded.
            */
            bool ReadDigitalOut(int index, uint16_t count, bool* out)
            {
                return ModbusIOHelper(&modbus_read_bits, index, count, (uint8_t*)out);
            }
            /*!
             * @brief Reads multiple digital outputs (bit).
             * @param index Output start address of modbus slave.
             * @param count Number of registers to read
             * @param value Array that should be set to the current input values.
             * @return True if the call succeeded.
            */
            bool ReadDigitalIn(int index, uint16_t count, bool* in)
            {
                return ModbusIOHelper(&modbus_read_input_bits, index, count, (uint8_t*)in);
            }

            /*!
             * @brief Writes multiple analog output values (Holding register).
             * @param index Start index of the analog output register.
             * @param count Number of registers to write
             * @param value Values that should be written
             * @return True if the call succeeded.
            */
            bool WriteAnalogOut(int index, uint16_t count, const uint16_t* values)
            {
                return ModbusIOHelper(&modbus_write_registers, index, count, values);
            }
            /*!
             * @brief Reads multiple analog output values (Holding register).
             * @param index Start index of the analog output register.
             * @param count Number of registers to read
             * @param out Array that should be set to the current output values.
             * @return True if the call succeeded.
            */
            bool ReadAnalogOut(int index, uint16_t count, uint16_t* out)
            {
                return ModbusIOHelper(&modbus_read_registers, index, count, out);
            }
            /*!
             * @brief Reads multiple analog input values (Holding register).
             * @param index Start index of the analog input register.
             * @param count Number of registers to read
             * @param out Array that should be set to the current input values.
             * @return True if the call succeeded.
            */
            bool ReadAnalogIn(int index, uint16_t count, uint16_t* in)
            {
                return ModbusIOHelper(&modbus_read_input_registers, index, count, in);
            }

        private:
            // Modbus IO function helper
            template<typename T, typename F, typename = std::enable_if_t<std::is_pointer_v<T> && std::is_invocable_r_v<int, F, modbus_t*, int, int, T>>>
            bool ModbusIOHelper(F func, int index, uint16_t count, T data)
            {
                bool result = false;
                if (count >= 1 && count <= 128)
                    Execute([&result, func, index, count, data](MSConnection& c)
                        {
                            result = func(c.Get(), index, (int)count, data) != -1;
                        }
                );
                return result;
            }

        private:
            modbus_t* m_ctx = nullptr;
            NetTools::IPV4Endpoint m_ctxEndpoint;
            int m_device = -1;
            bool m_connected = false;
    };
}
