#include "ProcessImage.h"

RETI::Modbus::PIBoolHandle::operator bool() const
{
    if (m_isInput)
    {
        return m_owner.InputByteAt(m_byteAddress) & (1 << m_bitAddress);
    }
    else
    {
        return m_owner.OutputByteAt(m_byteAddress) & (1 << m_bitAddress);
    }
}

RETI::Modbus::PIBoolHandle& RETI::Modbus::PIBoolHandle::operator=(bool value)
{
    if (m_isInput)
    {
        auto errorMessage = "Can't write to process image inputs!";
        GetLogger()->error(errorMessage);
        throw std::logic_error(errorMessage);
    }
    auto& byte = m_owner.OutputByteAt(m_byteAddress);
    byte ^= (-(uint8_t)value ^ byte) & (1UL << m_bitAddress);
    return *this;
}

RETI::Modbus::ProcessImage::ProcessImage(size_t inputSize, size_t outputSize)
{
    ResizePIMemory(&m_piInput, &m_piInputSize, inputSize);
    ResizePIMemory(&m_piOutput, &m_piOutputSize, outputSize);
    if (!m_piInput || !m_piOutput)
    {
        throw std::runtime_error("Failed to allocate memory for process image!");
    }
}

RETI::Modbus::ProcessImage::ProcessImage(ProcessImage&& other) noexcept
{
    // Copy
    m_piInput = other.m_piInput;
    m_piInputSize = other.m_piInputSize;
    m_piOutput = other.m_piOutput;
    m_piOutputSize = other.m_piOutputSize;

    // Invalidate
    other.m_piInput = nullptr;
    other.m_piOutput = nullptr;
}

RETI::Modbus::ProcessImage::ProcessImage(const ProcessImage& other)
{
    // Allocate 
    ResizePIMemory(&m_piInput, &m_piInputSize, other.m_piInputSize);
    ResizePIMemory(&m_piOutput, &m_piOutputSize, other.m_piOutputSize);
    if (!m_piInput || !m_piOutput)
    {
        throw std::runtime_error("Failed to allocate memory for process image!");
    }

    // Copy
    memcpy(m_piInput, other.m_piInput, m_piInputSize);
    memcpy(m_piOutput, other.m_piOutput, m_piOutputSize);
}

RETI::Modbus::ProcessImage& RETI::Modbus::ProcessImage::operator=(const ProcessImage& other)
{
    if (this != &other)
    {
        this->~ProcessImage();
        new(this)ProcessImage(other);
    }
    return *this;
}

RETI::Modbus::ProcessImage& RETI::Modbus::ProcessImage::operator=(ProcessImage&& other) noexcept
{
    if (this != &other)
    {
        this->~ProcessImage();
        new(this)ProcessImage(std::move(other));
    }
    return *this;
}

bool RETI::Modbus::ProcessImage::ResizePIMemory(uint8_t** ppMemory, size_t* oldSize, size_t newSize)
{
    // Allocate new
    uint8_t* newMemory = nullptr;
    if (*ppMemory)
    {
        // Resize
        newMemory = (uint8_t*)realloc(*ppMemory, newSize);
    }
    else
    {
        // New alloc
        newMemory = (uint8_t*)malloc(newSize);
    }

    // Update target
    if (newMemory)
    {
        memset(&newMemory[*oldSize], 0, newSize - *oldSize);
        *oldSize = newSize;
        *ppMemory = newMemory;
    }

    return newMemory != nullptr;
}

RETI::Modbus::ProcessImage::~ProcessImage()
{
    if (m_piInput) free(m_piInput);
    if (m_piOutput) free(m_piOutput);
}

bool RETI::Modbus::ProcessImage::EnsurePISize(size_t inputSize, size_t outputSize)
{
    // Input
    if (inputSize > m_piInputSize)
    {
        if (!ResizePIMemory(&m_piInput, &m_piInputSize, inputSize))
        {
            return false;
        }
    }

    // Output
    if (outputSize > m_piOutputSize)
    {
        if (!ResizePIMemory(&m_piOutput, &m_piOutputSize, outputSize))
        {
            return false;
        }
    }

    return true;
}

void RETI::Modbus::ProcessImage::CheckRange(size_t allocSize, size_t size, size_t index) const
{
    if (index >= allocSize || index + size >= allocSize)
    {
        GetLogger()->error("Can't access {} Bytes at location {}! Only {} Bytes allocated!", size, index, allocSize);
        throw std::range_error("Illegal process image range access!");
    }
}

void RETI::Modbus::ProcessImage::CheckRangeBit(size_t allocSize, size_t size, size_t index, int8_t bit) const
{
    CheckRange(allocSize, size, index);
    if (bit < 0 || bit > 7)
    {
        GetLogger()->error("Invalid bit index {} (Must be in between 0 and 7)", bit);
        throw std::range_error("Illegal process image range access!");
    }
}

void RETI::Modbus::ProcessImage::AllOutputsLow()
{
    if (m_piOutput)
    {
        memset(m_piOutput, 0, m_piOutputSize);
    }
}
