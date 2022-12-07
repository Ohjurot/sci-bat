#include "SMAData.h"

float SCI::BAT::Gateway::SMAConvertFromFix(int32_t value, int fixBits)
{
    return (float)value / fixBits * 10;
}
