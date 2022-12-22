 /*!
  * @file Master.h
  * @brief Modbus master that manages the process image and all slaves
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma once

#include <ModbusMaster/Slave.h>
#include <ModbusMaster/IOHandle.h>
#include <ModbusMaster/ProcessImage.h>

#include <NetTools/IPV4.h>
#include <SCIUtil/SPDLogable.h>

#include <fmt/format.h>
#include <scn/scn.h>

#include <unordered_map>
#include <string>
#include <string_view>

#ifdef SCI_WINDOWS
#define SCI_MODBUS_ENDIAN SCI::Modbus::Endianness::Little
#else
#define SCI_MODBUS_ENDIAN SCI::Modbus::Endianness::Big 
#endif

namespace SCI::Modbus
{
    /*!
     * @brief Type of endianness
    */
    enum class Endianness
    {
        /*! Little Endian */
        Little,
        /*! Bit Endian */
        Big,
    };

    /*!
     * @brief Modbus master
     * 
     * Modbus master implementation. Manages all slaves, the process image and aliases.
    */
    class Master : public Util::SPDLogable
    {
        public:
            /*!
             * @brief Type of io (input/output).
            */
            enum class IOType
            {
                /*! Input */
                Input,
                /*! Output */
                Output
            };

        public:
            Master() = default;
            /*!
             * @brief Creates a new master with the give sizes for the process image.
             * @param piInputSize Size (in bytes) of the input process image.
             * @param piOutputSize Size (in bytes) of the output process image.
            */
            Master(size_t piInputSize, size_t piOutputSize) :
                m_processImage(piInputSize, piOutputSize)
            {};
            Master(const Master&) = delete;
            Master(Master&& other) noexcept = default;

            Master& operator=(const Master&) = delete;
            Master& operator=(Master&& other) noexcept = default;

            /*!
             * @brief Sets up a new modbus slave.
             * @param name Desired name of the new slave. Function will throw if slave already exists!
             * @param endpoint TCP/IP endpoint of modbus slave.
             * @param slaveId Modbus ID to be applied for the slave. Set to -1 if unused.
             * @return Reference to the slave for sub sequential configuration.
            */
            SCI::Modbus::Slave& SetupSlave(const std::string& name, NetTools::IPV4Endpoint& endpoint, int slaveId = -1);

            /*!
             * @brief Accesses an existing slave for modification.
             * @param name Name of the slave to be accessed. Will throw if slave is not present.
             * @return Reference to slave.
            */
            SCI::Modbus::Slave& SetupSlave(const std::string& name);

            /*!
             * @brief Registers an alias for the given address.
             * @param addr Address to which the alias should resolve.
             * @param alias Alias.
             * @return Reference to self.
            */
            inline Master& Alias(const std::string& addr, const std::string& alias)
            {
                m_aliasMapping[alias] = addr;
                return *this;
            }

            /*!
             * @brief Access the data at a give address / alias via the returned output handle.
             * @param name Address or alias. 
             * @return Output handle for requested address / alias.
            */
            SCI::Modbus::IOHandle At(const std::string_view& name);
            /*!
             * @brief Access data of a resolved address via the returned output handle.
             * @param type Input/Output type of data.
             * @param dtype Data type to be accessed.
             * @param byteAddress Offset of the requested memory in bytes.
             * @param bitAddress Bit offset of the requested memory into byte. Only valid id dtype is Bit.
             * @return Output handle based on parameters.
            */
            SCI::Modbus::IOHandle At(IOType type, IOHandle::DataType dtype, size_t byteAddress, uint8_t bitAddress);
            inline SCI::Modbus::IOHandle operator[](const std::string_view& name)
            {
                return At(name);
            }

            /*!
             * @brief Sets the current valid endianness. Valid after this call until is subsequential call to SetEndianness() or SetSwapEndianness().
             * @param ed Endianness to be applied.
            */
            inline void SetEndianness(Endianness ed)
            {
                m_swapEndian = ed != SCI_MODBUS_ENDIAN;
            }
            /*!
             * @brief Indicates if endianness should be swaped. Valid after this call until is subsequential call to SetEndianness() or SetSwapEndianness().
             * @param swap True if endianness shall be swap
            */
            inline void SetSwapEndianness(bool swap)
            {
                m_swapEndian = swap;
            }

            /*!
             * @brief Updates all slaves.
             * @param deltaT Delta time since last update.
             * @return true if all slaved updated successfully.
            */
            bool IOUpdate(float deltaT);

            /*!
             * @brief Gain access to the process image.
             * @return Reference to internal process imaage.
            */
            inline ProcessImage& GetProcessImage()
            {
                return m_processImage;
            }

            /*!
             * @brief Checks if a slave is currently connected.
             * @param name Name of slave to be checkd.
             * @return True if currently connected.
            */
            inline bool SlaveConnected(const std::string& name)
            {
                auto it = m_slaves.find(name);
                return it != m_slaves.end() ? it->second.GetLastUpdateOk() : false;
            }

        private:
            static bool ParseAddressString(const std::string_view& addressString, IOType& type, IOHandle::DataType& dtype, size_t& byteAddress, uint8_t& bitAddress);

        private:
            std::unordered_map<std::string, std::string> m_aliasMapping;
            std::unordered_map<std::string, Slave> m_slaves;
            ProcessImage m_processImage;

            bool m_swapEndian = false;
    };
}
