#include "SMAData.h"

float SCI::BAT::Gateway::SMAConvertFromFix(int32_t value, int fixBits)
{
    return (float)value / std::pow(10., fixBits);
}
