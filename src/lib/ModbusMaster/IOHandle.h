/*
 *      Handle that allows checked modifications to the process image
 *
 *      Author: Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <ModbusMaster/ProcessImage.h>

#include <cstdint>

namespace SCI::Modbus
{
    class IOHandle
    {
        public:
            enum class DataType : uint8_t
            {
                None = 0,
                Bit = 1, 
                Byte = 8, 
                Word = 16, 
                DWord = 32, 
                QWord = 64,
            };

        public:
            IOHandle() = delete;
            IOHandle(const IOHandle&) = default;
            IOHandle(IOHandle&&) noexcept = default;
            IOHandle(ProcessImage& processImage, DataType type, bool isInput, size_t byteOffset, uint8_t bitOffset = 0, bool swapEndian = false) :
                m_pi(processImage), m_type(type), m_isInput(isInput), m_byteOffset(byteOffset), m_bitOffset(bitOffset), m_swapEndian(swapEndian)
            { }

            IOHandle& operator=(const IOHandle&) = default;
            IOHandle& operator=(IOHandle&&) noexcept = default;

            // Get functions
            bool GetBitValue() const
            {
                return BufferHelper()[m_byteOffset] & (1UL << m_bitOffset);
            }
            inline int8_t GetByteValue() const
            {
                return GetHelper<int8_t>();
            }
            inline int16_t GetWordValue() const
            {
                return GetHelper<int16_t>();
            }
            inline int32_t GetDWordValue() const
            {
                return GetHelper<int32_t>();
            }
            inline int64_t GetQWordValue() const
            {
                return GetHelper<int64_t>();
            }

            // Get operator
            inline operator bool() const { return GetBitValue(); }
            inline operator int8_t() const { return GetByteValue(); }
            inline operator int16_t() const { return GetWordValue(); }
            inline operator int32_t() const { return GetDWordValue(); }
            inline operator int64_t() const { return GetQWordValue(); }

            // Set functions
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
            inline void SetByteValue(int8_t value)
            {
                SetHelper(value);
            }
            inline void SetWordValue(int16_t value)
            {
                SetHelper(value);
            }
            inline void SetDWordValue(int32_t value)
            {
                SetHelper(value);
            }
            inline void SetQWordValue(int64_t value)
            {
                SetHelper(value);
            }

            // Set operator
            inline IOHandle& operator=(bool value)
            {
                SetBitValue(value);
                return *this;
            }
            inline IOHandle& operator=(int8_t value)
            {
                SetByteValue(value);
                return *this;
            }
            inline IOHandle& operator=(int16_t value)
            {
                SetWordValue(value);
                return *this;
            }
            inline IOHandle& operator=(int32_t value)
            {
                SetDWordValue(value);
                return *this;
            }
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
