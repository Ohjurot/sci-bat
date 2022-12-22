 /*!
  * @file Slave.h
  * @brief Class that connects to and maps IO from a modbus slave
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma once

#include <ModbusMaster/MSConnection.h>
#include <ModbusMaster/ProcessImage.h>

#include <SCIUtil/SPDLogable.h>

#include <fmt/format.h>

#include <vector>
#include <string>
#include <sstream>

namespace SCI::Modbus
{
    /*!
     * @brief Defines a single slave.
    */
    class Slave : public Util::SPDLogable
    {
        public:
            /*!
             * @brief Modbus sided mapping type.
            */
            enum class RemoteMappingType
            {
                /*! Digital Input. */
                DigitalInput,
                /*! Digital Output. */
                DigitalOutput,
                /*! Analog Input. */
                AnalogInput,
                /*! Analog Output. */
                AnalogOutput,
            };

            /*!
             * @brief Maps local and remote registers.
            */
            struct Mapping
            {
                /*!
                 * @brief Remove mapping configuration.
                */
                struct
                {
                    /*! Type of remote mapping. */
                    RemoteMappingType type;
                    /*! Remote start address. */
                    int startAddess;
                    /*! Number of remote bits / registers. */
                    uint16_t count;
                } Remote;

                /*!
                 * @brief Local mapping configuration.
                */
                struct
                {
                    /*! Local byte offset into process image. */
                    size_t byteOffset;
                    /*! Local bit offset into byte. Only valid if remote type is digital. */
                    uint8_t bitOffset;
                } Local;
            };

            /*!
             * @brief Result of a slave update call.
            */
            enum class IOUpdateResult
            {
                /*! Slave was invalid. */
                InvalidSlave,
                /*! Connection to slave failed. */
                ConnectionError,
                /*! At minimum one mapping could not be updated. */
                UpdateFailed,
                /*! All mappings where updated successfully. */
                UpdateSuccess,
                
                /*! Slave is in connection delay after connection failure. */
                FailedConnectionDelay,
                /*! Slave connection is still failing / retry failed. */
                ConnectionStilFailing,
                /*! Slave connection is restored and updates were successfully. */
                ConnectionRestoredAndSuccess,
            };

        public:
            Slave() = default;
            /*!
             * @brief Creates a new slave bound to port and device id.
             * @param endpoint TCP/IP Endpoint of the slave to connect to.
             * @param deviceId Device id of slave.
            */
            Slave(const SCI::NetTools::IPV4Endpoint& endpoint, int deviceId = -1) : 
                m_valid(true), m_connection(endpoint, deviceId)
            {};
            Slave(const Slave&) = delete;
            Slave(Slave&& other) noexcept;
            
            Slave& operator=(const Slave&) = delete;
            Slave& operator=(Slave&& other) noexcept;

            /*!
             * @brief Updates the connection details of the slave
             * @param endpoint New TCP/IP Endpoint of the slave to connect to.
             * @param deviceId New device id of slave.
            */
            void UpdateConnection(const SCI::NetTools::IPV4Endpoint& endpoint, int deviceId = -1);

            /*!
             * @brief Registers an IO mapping for the slave.
             * @param mapping Input mapping object.
             * @return Reference to self.
            */
            Slave& Map(const Mapping& mapping);
            /*!
             * @brief Registers an IO mapping for the slave.
             * @param mapping Input mapping object.
             * @return Reference to self.
            */
            Slave& Map(Mapping&& mapping);
            /*!
             * @brief Registers an IO mapping for the slave.
             * @param remoteType Type of remote mapping.
             * @param remoteAddress Remote bit / register address.
             * @param count Remote number of bits / registers.
             * @param localByteAddress Local mapping byte offset.
             * @param localBitAddress Local mapping bit offset. Only valid when remoteType is digital.
             * @return Reference to self.
            */
            inline Slave& Map(RemoteMappingType remoteType, int remoteAddress, uint16_t count, size_t localByteAddress, uint8_t localBitAddress = 0)
            {
                return Map({remoteType, remoteAddress, count, localByteAddress, localBitAddress });
            }

            /*!
             * @brief Executes an IO update. Will read inputs and write outputs as configured in mappings.
             * @param processImage Input / output process image.
             * @param deltaT Delta time since last update. Used for connection delay.
             * @return Result of operation.
            */
            SCI::Modbus::Slave::IOUpdateResult ExecuteIOUpdate(ProcessImage& processImage, float deltaT);

            /*!
             * @brief Calls ExecuteIOUpdate().
             * @return Result of operation.
            */
            inline IOUpdateResult operator()(ProcessImage& processImage, float deltaT)
            {
                return ExecuteIOUpdate(processImage, deltaT);
            }

            /*!
             * @brief Checks if the last slave update was successfully without any errors.
             * @return True if successfully.
            */
            inline bool GetLastUpdateOk() const
            {
                return m_lastUpdateOk;
            }

        private:
            void ValidateMapping(const Mapping& mapping) const;

        private:
            bool m_valid = false;
            MSConnection m_connection;
            std::vector<Mapping> m_mappings;

            bool m_lastUpdateOk = false;

            const uint8_t m_subsequentConnectionTimeoutsThreshold = 5;
            uint8_t m_subsequentConnectionTimeouts = 0;
            const float m_connectionRetryTimerPreset = 5.f;
            float m_connectionRetryTimer = .0f;
    };
}
