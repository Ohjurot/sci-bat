 /*!
  * @file ProcessImage.h
  * @brief In memory process image (IO) of all modbus slaves.
  * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
  */
#pragma once

#include <SCIUtil/SPDLogable.h>

#include <fmt/format.h>

#include <cstdlib>
#include <cstdint>
#include <stdexcept>

namespace SCI::Modbus
{
    class ProcessImage;

    /*!
     * @brief Handle that allows editing boolean values inside the process image.
    */
    class PIBoolHandle
    {
        public:
            PIBoolHandle() = delete;
            /*!
             * @brief Creates a new handle with given offsets.
             * @param owner Process image that is owning the data.
             * @param byteAddress Byte offset of the value insided the process image.
             * @param bitAddress Bit offset of the value inside the byte.
             * @param isInput True if handle is an input (read only) handle.
            */
            inline PIBoolHandle(ProcessImage& owner, size_t byteAddress, int8_t bitAddress, bool isInput) :
                m_owner(owner), m_byteAddress(byteAddress), m_bitAddress(bitAddress), m_isInput(isInput)
            {}
            PIBoolHandle(const PIBoolHandle&) = delete;
            PIBoolHandle(PIBoolHandle&&) noexcept = default;

            PIBoolHandle& operator=(const PIBoolHandle&) = delete;
            PIBoolHandle& operator=(PIBoolHandle&&) noexcept = default;

            /*!
             * @brief Conversion to boolean. Will return the current state of the bit / boolean (inside process image).
            */
            operator bool() const;
            /*!
             * @brief Assigns a new value to the targeted value inside the process image.
             * 
             * Will throw if handle is configured as a input handle!
             * @param value Value to be written.
             * @return Reference to self.
            */
            PIBoolHandle& operator=(bool value);

        private:
            ProcessImage& m_owner;
            size_t m_byteAddress = 0;
            int8_t m_bitAddress = -1;
            bool m_isInput = false;
    };

    /*!
     * @brief Holds all the process data.
    */
    class ProcessImage
    {
        public:
            ProcessImage() = default;
            /*!
             * @brief Creates a new process image with the specified sizes.
             * @param inputSize Size (in bytes) of the input process image.
             * @param outputSize Size (in bytes) of the output process image.
            */
            ProcessImage(size_t inputSize, size_t outputSize);
            ProcessImage(const ProcessImage& other);
            ProcessImage(ProcessImage&& other) noexcept;
            ~ProcessImage();

            ProcessImage& operator=(const ProcessImage& other);
            ProcessImage& operator=(ProcessImage&& other) noexcept;

            /*!
             * @brief Ensures sufficient size of the process image. Will resize process image if required. 
             * @param inputSize New required size of the input process image in bytes.
             * @param outputSize New required size of the output process image in bytes. 
             * @return True if size could be ensured.
            */
            bool EnsurePISize(size_t inputSize, size_t outputSize);

            /*!
             * @brief Writes all bits in the output process inputs outputs to low (memset() to 0x00).
            */
            void AllOutputsLow();

            /*!
             * @brief Retrieves the size of the input process image.
             * @return Size in bytes.
            */
            inline size_t GetInputSize() const noexcept
            {
                return m_piInputSize;
            }
            /*!
             * @brief Retrieves the size of the output process image.
             * @return Size in bytes.
            */
            inline size_t GetOutputSize() const noexcept
            {
                return m_piOutputSize;
            }

            /*!
             * @brief Gain access to the input process image (buffer).
             * @return Pointer to process image buffer.
            */
            inline uint8_t* GetInputBuffer() noexcept
            {
                return m_piInput;
            }
            /*!
             * @brief Gain access to the output process image (buffer).
             * @return Pointer to process image buffer.
            */
            inline uint8_t* GetOutputBuffer() noexcept
            {
                return m_piOutput;
            }

            /*!
             * @brief Access an input bit at given offsets.
             * @param offset Byte offset into input process image.
             * @param bit Bit offset into byte.
             * @return Boolean access handle.
            */
            inline PIBoolHandle InputBitAt(size_t offset, int8_t bit)
            {
                CheckRangeBit(m_piInputSize, sizeof(uint8_t), offset, bit);
                return PIBoolHandle(*this, offset, bit, true);
            }
            /*!
             * @brief Access an output bit at given offsets.
             * @param offset Byte offset into output process image.
             * @param bit Bit offset into byte.
             * @return Boolean access handle.
            */
            inline PIBoolHandle OutputBitAt(size_t offset, int8_t bit)
            {
                CheckRangeBit(m_piInputSize, sizeof(uint8_t), offset, bit);
                return PIBoolHandle(*this, offset, bit, false);
            }

            /*!
             * @brief Access an input byte at given offsets.
             * @param offset Offset into the input process images in byte.
             * @return Current input value.
            */
            inline uint8_t InputByteAt(size_t offset) const
            {
                CheckRange(m_piInputSize, sizeof(uint8_t), offset);
                return m_piInput[offset];
            }
            /*!
             * @brief Access an input word at given offsets.
             * @param offset Offset into the input process images in byte.
             * @return Current input value.
            */
            inline uint16_t InputWordAt(size_t offset) const
            {
                CheckRange(m_piInputSize, sizeof(uint16_t), offset);
                return *((uint16_t*)&m_piInput[offset]);
            }
            /*!
             * @brief Access an input dword at given offsets.
             * @param offset Offset into the input process images in byte.
             * @return Current input value.
            */
            inline uint32_t InputDWordAt(size_t offset) const
            {
                CheckRange(m_piInputSize, sizeof(uint32_t), offset);
                return *((uint32_t*)&m_piInput[offset]);
            }
            /*!
             * @brief Access an input qword at given offsets.
             * @param offset Offset into the input process images in byte.
             * @return Current input value.
            */
            inline uint64_t InputQWordAt(size_t offset) const
            {
                CheckRange(m_piInputSize, sizeof(uint64_t), offset);
                return *((uint64_t*)&m_piInput[offset]);
            }

            /*!
             * @brief Access an output byte at given offsets.
             * @param offset Offset into the output process images in byte.
             * @return Modifiable reference to current output value.
            */
            inline uint8_t& OutputByteAt(size_t offset)
            {
                CheckRange(m_piOutputSize, sizeof(uint8_t), offset);
                return m_piOutput[offset];
            }
            /*!
             * @brief Access an output word at given offsets.
             * @param offset Offset into the output process images in byte.
             * @return Modifiable reference to current output value.
            */
            inline uint16_t& OutputWordAt(size_t offset)
            {
                CheckRange(m_piOutputSize, sizeof(uint16_t), offset);
                return *((uint16_t*)&m_piOutput[offset]);
            }
            /*!
             * @brief Access an output dword at given offsets.
             * @param offset Offset into the output process images in byte.
             * @return Modifiable reference to current output value.
            */
            inline uint32_t& OutputDWordAt(size_t offset)
            {
                CheckRange(m_piOutputSize, sizeof(uint32_t), offset);
                return *((uint32_t*)&m_piOutput[offset]);
            }
            /*!
             * @brief Access an output qword at given offsets.
             * @param offset Offset into the output process images in byte.
             * @return Modifiable reference to current output value.
            */
            inline uint64_t& OutputQWordAt(size_t offset)
            {
                CheckRange(m_piOutputSize, sizeof(uint64_t), offset);
                return *((uint64_t*)&m_piOutput[offset]);
            }

        private:
            static bool ResizePIMemory(uint8_t** ppMemory, size_t* oldSize, size_t newSize);
            void CheckRange(size_t allocSize, size_t size, size_t index) const;
            void CheckRangeBit(size_t allocSize, size_t size, size_t index, int8_t bit) const;

        private:
            uint8_t* m_piInput = nullptr;
            size_t m_piInputSize = 0;

            uint8_t* m_piOutput = nullptr;
            size_t m_piOutputSize = 0;
    };
}
