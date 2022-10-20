#pragma once

#include <RETIUtil/SPDLogable.h>

#include <fmt/format.h>

#include <cstdlib>
#include <cstdint>
#include <stdexcept>

namespace RETI::Modbus
{
    class ProcessImage;

    // Handle for editing boolean values inside the pa
    class PIBoolHandle
    {
        public:
            PIBoolHandle() = delete;
            inline PIBoolHandle(ProcessImage& owner, size_t byteAddress, int8_t bitAddress, bool isInput) :
                m_owner(owner), m_byteAddress(byteAddress), m_bitAddress(bitAddress), m_isInput(isInput)
            {}
            PIBoolHandle(const PIBoolHandle&) = delete;
            PIBoolHandle(PIBoolHandle&&) noexcept = default;

            PIBoolHandle& operator=(const PIBoolHandle&) = delete;
            PIBoolHandle& operator=(PIBoolHandle&&) noexcept = default;

            operator bool() const;
            PIBoolHandle& operator=(bool value);

        private:
            ProcessImage& m_owner;
            size_t m_byteAddress = 0;
            int8_t m_bitAddress = -1;
            bool m_isInput = false;
    };

    class ProcessImage
    {
        public:
            ProcessImage() = default;
            ProcessImage(size_t inputSize, size_t outputSize);
            ProcessImage(const ProcessImage& other);
            ProcessImage(ProcessImage&& other) noexcept;
            ~ProcessImage();

            ProcessImage& operator=(const ProcessImage& other);
            ProcessImage& operator=(ProcessImage&& other) noexcept;

            bool EnsurePISize(size_t inputSize, size_t outputSize);
            void AllOutputsLow();

            // Sizes
            inline size_t GetInputSize() const noexcept
            {
                return m_piInputSize;
            }
            inline size_t GetOutputSize() const noexcept
            {
                return m_piOutputSize;
            }

            // Raw pointers
            inline uint8_t* GetInputBuffer() noexcept
            {
                return m_piInput;
            }
            inline uint8_t* GetOutputBuffer() noexcept
            {
                return m_piOutput;
            }

            // Access boolean (with handle)
            inline PIBoolHandle InputBitAt(size_t offset, int8_t bit)
            {
                CheckRangeBit(m_piInputSize, sizeof(uint8_t), offset, bit);
                return PIBoolHandle(*this, offset, bit, true);
            }
            inline PIBoolHandle OutputBitAt(size_t offset, int8_t bit)
            {
                CheckRangeBit(m_piInputSize, sizeof(uint8_t), offset, bit);
                return PIBoolHandle(*this, offset, bit, false);
            }

            // Access full bytes (Input)
            inline const uint8_t& InputByteAt(size_t offset) const
            {
                CheckRange(m_piInputSize, sizeof(uint8_t), offset);
                return m_piInput[offset];
            }
            const uint16_t& InputWordAt(size_t offset) const
            {
                CheckRange(m_piInputSize, sizeof(uint16_t), offset);
                return *((uint16_t*)&m_piInput[offset]);
            }
            const uint32_t& InputDWordAt(size_t offset) const
            {
                CheckRange(m_piInputSize, sizeof(uint32_t), offset);
                return *((uint32_t*)&m_piInput[offset]);
            }
            const uint64_t& InputQWordAt(size_t offset) const
            {
                CheckRange(m_piInputSize, sizeof(uint64_t), offset);
                return *((uint64_t*)&m_piInput[offset]);
            }

            // Access full bytes (Input)
            inline uint8_t& OutputByteAt(size_t offset)
            {
                CheckRange(m_piOutputSize, sizeof(uint8_t), offset);
                return m_piOutput[offset];
            }
            inline uint16_t& OutputWordAt(size_t offset)
            {
                CheckRange(m_piOutputSize, sizeof(uint16_t), offset);
                return *((uint16_t*)&m_piOutput[offset]);
            }
            inline uint32_t& OutputDWordAt(size_t offset)
            {
                CheckRange(m_piOutputSize, sizeof(uint32_t), offset);
                return *((uint32_t*)&m_piOutput[offset]);
            }
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
