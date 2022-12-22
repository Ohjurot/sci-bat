 /*! 
  * @file IOHandle.h
  * @brief Handle that allows checked modifications to the process image
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma once

#include <ModbusMaster/ProcessImage.h>

#include <cstdint>

namespace SCI::Modbus
{
    /*!
     * @brief Modbus value proxy
     * 
     * Proxy for editing a modbus value (read and write depending on handle)
    */
    class IOHandle
    {
        public:
            /*!
             * @brief Modbus data type
             * 
             * Indicated what type of value can be accessed with the handle
            */
            enum class DataType : uint8_t
            {
                /*! No data type (invalid handle) */
                None = 0, 
                /*! Bit (Input / Output bit) */
                Bit = 1,  
                /*! Byte (8Bit value) */
                Byte = 8, 
                /*! Word (16Bit value) */
                Word = 16, 
                /*! Double World (32Bit value) */
                DWord = 32, 
                /*! Quad Word (64Bit value) */
                QWord = 64, 
            };

        public:
            IOHandle() = delete;
            IOHandle(const IOHandle&) = default;
            IOHandle(IOHandle&&) noexcept = default;
            /*!
             * @brief Creates a valid IOHandle into the supplied process image with given type, offset and modifiers.
             * @param processImage Reference to process image holding the byte(s) to be modified by this handle
             * @param type Data type of the handle (how many bits/bytes to be modified by handle)
             * @param isInput Indicates if the data targeted by the handle is an input. Indicates if data is targeted in input (true) or output (false) process image. If true modifications will be disallowed!
             * @param byteOffset Offset of the target value in byte. Offset into the process image.
             * @param bitOffset Bit offset of the target in the offset byte. Only valid if type is Bit
             * @param swapEndian If true all read and write will swap the endianness. Performs 16Bit swaps (protocol native). Withing the 16Bits the endianness conversion is done automatically by the modbus library. 
            */
            IOHandle(ProcessImage& processImage, DataType type, bool isInput, size_t byteOffset, uint8_t bitOffset = 0, bool swapEndian = false) :
                m_pi(processImage), m_type(type), m_isInput(isInput), m_byteOffset(byteOffset), m_bitOffset(bitOffset), m_swapEndian(swapEndian)
            { }

            IOHandle& operator=(const IOHandle&) = default;
            IOHandle& operator=(IOHandle&&) noexcept = default;

            /*!
             * @brief Access the handles data as bit.
             * @return Value of the target data as bit.
            */
            bool GetBitValue() const
            {
                return BufferHelper()[m_byteOffset] & (1UL << m_bitOffset);
            }
            /*!
             * @brief Access the handles data as byte.
             * @return Value of the target data in signed 8Bit format.
            */
            inline int8_t GetByteValue() const
            {
                return GetHelper<int8_t>();
            }
            /*!
             * @brief Access the handles data as word.
             * @return Value of the target data in signed 16Bit format.
            */
            inline int16_t GetWordValue() const
            {
                return GetHelper<int16_t>();
            }
            /*!
             * @brief Access the handles data as dword.
             * @return Value of the target data in signed 32Bit format.
            */
            inline int32_t GetDWordValue() const
            {
                return GetHelper<int32_t>();
            }
            /*!
             * @brief Access the handles data as qword.
             * @return Value of the target data in signed 64Bit format.
            */
            inline int64_t GetQWordValue() const
            {
                return GetHelper<int64_t>();
            }

            /*!
             * @brief Calls GetBitValue().
            */
            inline operator bool() const { return GetBitValue(); }
            /*!
             * @brief Calles GetByteValue().
            */
            inline operator int8_t() const { return GetByteValue(); }
            /*!
             * @brief Calles GetWordValue().
            */
            inline operator int16_t() const { return GetWordValue(); }
            /*!
             * @brief Calles GetDWordValue().
            */
            inline operator int32_t() const { return GetDWordValue(); }
            /*!
             * @brief Calles GetQWordValue().
            */
            inline operator int64_t() const { return GetQWordValue(); }

            /*!
             * @brief Writes a bit value to the data targeted by handle.
             * @param value Input value as state of bit (bool).
            */
            void SetBitValue(bool value)
            {
                // Assert output
                if (m_isInput)
                {
                    throw std::runtime_error("Write access to input (read-only) data is not allowed!");
                }

                // Write
                auto& target = BufferHelper()[m_byteOffset];
                target ^= (-(uint8_t)value ^ target) & (1UL << m_bitOffset);
            }
            /*!
             * @brief Writes a byte value to the data targeted by handle.
             * @param value Input value as signed 8Bit integer.
            */
            inline void SetByteValue(int8_t value)
            {
                SetHelper(value);
            }
            /*!
             * @brief Writes a word value to the data targeted by handle.
             * @param value Input value as signed 16Bit integer.
            */
            inline void SetWordValue(int16_t value)
            {
                SetHelper(value);
            }
            /*!
             * @brief Writes a dword value to the data targeted by handle.
             * @param value Input value as signed 32Bit integer.
            */
            inline void SetDWordValue(int32_t value)
            {
                SetHelper(value);
            }
            /*!
             * @brief Writes a qword value to the data targeted by handle.
             * @param value Input value as signed 64Bit integer.
            */
            inline void SetQWordValue(int64_t value)
            {
                SetHelper(value);
            }

            /*!
             * @brief Calls SetBitValue().
             * @param value Forwarded value
             * @return Reference to self
            */
            inline IOHandle& operator=(bool value)
            {
                SetBitValue(value);
                return *this;
            }
            /*!
             * @brief Calls SetByteValue().
             * @param value Forwarded value
             * @return Reference to self
            */
            inline IOHandle& operator=(int8_t value)
            {
                SetByteValue(value);
                return *this;
            }
            /*!
             * @brief Calls SetWordValue().
             * @param value Forwarded value
             * @return Reference to self
            */
            inline IOHandle& operator=(int16_t value)
            {
                SetWordValue(value);
                return *this;
            }
            /*!
             * @brief Calls SetDWordValue().
             * @param value Forwarded value
             * @return Reference to self
            */
            inline IOHandle& operator=(int32_t value)
            {
                SetDWordValue(value);
                return *this;
            }
            /*!
             * @brief Calls SetQWordValue().
             * @param value Forwarded value
             * @return Reference to self
            */
            inline IOHandle& operator=(int64_t value)
            {
                SetQWordValue(value);
                return *this;
            }

        private:
            template<typename T>
            T GetHelper() const
            {
                if (sizeof(T) != (size_t)m_type / 8)
                {
                    throw std::runtime_error("Type mismatch!");
                }

                return m_swapEndian ? SwapEndian<T>(*((T*)&BufferHelper()[m_byteOffset])) : *((T*)&BufferHelper()[m_byteOffset]);
            }
            
            template<typename T>
            void SetHelper(const T& value)
            {
                if (sizeof(T) != (size_t)m_type / 8)
                {
                    throw std::runtime_error("Type mismatch!");
                }

                if (m_isInput)
                {
                    throw std::runtime_error("Write access to input (read-only) data is not allowed!");
                }

                *((T*)&BufferHelper()[m_byteOffset]) = m_swapEndian ? SwapEndian<T>(value) : value;
            }

            const uint8_t* BufferHelper() const 
            {
                return m_isInput ? m_pi.GetInputBuffer() : m_pi.GetOutputBuffer();
            }
            uint8_t* BufferHelper()
            {
                return m_isInput ? m_pi.GetInputBuffer() : m_pi.GetOutputBuffer();
            }

            template<typename T>
            static T SwapEndian(T u)
            {
                // Union for conversion
                union
                {
                    T u;
                    uint16_t u16[sizeof(T)];
                } src, dst;
                
                // Byte swap
                src.u = u;
                for (size_t i = 0; i < sizeof(T) / 2; i++)
                {
                    dst.u16[i] = src.u16[sizeof(T) / 2 - 1 - i];
                }
                return dst.u;
            }

        private:
            ProcessImage& m_pi;

            bool m_swapEndian = false;
            
            DataType m_type = DataType::None;
            bool m_isInput = false;
            size_t m_byteOffset = 0;
            uint8_t m_bitOffset = 0;
    };
}
