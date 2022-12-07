#pragma once

#include <cstdint>

namespace SCI::BAT::Gateway
{
    enum class SMAStatus
    {
        Undefined = 0,
        Error = 35, 
        Off = 303, 
        Ok = 307,
        Warning = 455,
    };

    enum class SMABatteryStatus
    {
        Undefined = 0,
        OwnConsumptionArea = 2614, 
        StateOfChargeConservationArea = 2615,
        BackupPowerArea = 2616, 
        DeepDischargeProtectionArea = 2617,
        DeepDischargeArea = 2618,
    };

    enum class SMAOperationStatus
    {
        Undefined = 0,
        Stop = 381,
        Start = 1467,
    };

    enum class SMABatteryType
    {
        Undefined = 0,
        ValveRegulatedLeadAcid = 1782,
        FloodedLeadAcid = 1783,
        LithiumIon = 1785,
    };

    struct SMAInData
    {
        SMAStatus status = SMAStatus::Undefined;
        int32_t power = 0;
        float voltage = .0f;
        float freqenency = .0f;
        float batteryCurrent = .0f;
        int8_t batteryCharge = 0;
        int8_t batteryCapacity = 0;
        float batteryTemperature = .0f;
        float batteryVoltage = .0f;
        uint32_t timeUntilFullCharge = -1;
        uint32_t timeUntilFullDischarge = -1;
        SMABatteryStatus batteryStatus = SMABatteryStatus::Undefined;
        SMAOperationStatus operationStaus = SMAOperationStatus::Undefined;
        SMABatteryType batteryType = SMABatteryType::Undefined;
        uint32_t serialNumber = -1;
    };

    struct SMAOutData
    {
        bool enablePowerControle = false;
        int32_t power = 0;
    };
}
