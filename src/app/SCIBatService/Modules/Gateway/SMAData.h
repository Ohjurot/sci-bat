#pragma once

#include <fmt/format.h>

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

    float SMAConvertFromFix(int32_t value, int fixBits);

    
}

template <> struct fmt::formatter<SCI::BAT::Gateway::SMAStatus>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const SCI::BAT::Gateway::SMAStatus& lhs, FormatContext& ctx) const -> decltype(ctx.out())
    {
        switch (lhs)
        {
            case SCI::BAT::Gateway::SMAStatus::Ok:
                return fmt::format_to(ctx.out(), "Ok");
            case SCI::BAT::Gateway::SMAStatus::Off:
                return fmt::format_to(ctx.out(), "Off");
            case SCI::BAT::Gateway::SMAStatus::Error:
                return fmt::format_to(ctx.out(), "Error");
            case SCI::BAT::Gateway::SMAStatus::Warning:
                return fmt::format_to(ctx.out(), "Warning");
            case SCI::BAT::Gateway::SMAStatus::Undefined:
            default:
                return fmt::format_to(ctx.out(), "Information not available");
        }
    }
};

template <> struct fmt::formatter<SCI::BAT::Gateway::SMABatteryStatus>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const SCI::BAT::Gateway::SMABatteryStatus& lhs, FormatContext& ctx) const -> decltype(ctx.out())
    {
        switch (lhs)
        {
            case SCI::BAT::Gateway::SMABatteryStatus::OwnConsumptionArea:
                return fmt::format_to(ctx.out(), "Own consumption area");
            case SCI::BAT::Gateway::SMABatteryStatus::StateOfChargeConservationArea:
                return fmt::format_to(ctx.out(), "State of charge conservation area");
            case SCI::BAT::Gateway::SMABatteryStatus::BackupPowerArea:
                return fmt::format_to(ctx.out(), "Backup power area");
            case SCI::BAT::Gateway::SMABatteryStatus::DeepDischargeProtectionArea:
                return fmt::format_to(ctx.out(), "Deep discharge protection area");
            case SCI::BAT::Gateway::SMABatteryStatus::DeepDischargeArea:
                return fmt::format_to(ctx.out(), "Deep discharge area");
            case SCI::BAT::Gateway::SMABatteryStatus::Undefined:
            default:
                return fmt::format_to(ctx.out(), "Information not available");
        }
    }
};

template <> struct fmt::formatter<SCI::BAT::Gateway::SMAOperationStatus>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const SCI::BAT::Gateway::SMAOperationStatus& lhs, FormatContext& ctx) const -> decltype(ctx.out())
    {
        switch (lhs)
        {
            case SCI::BAT::Gateway::SMAOperationStatus::Stop:
                return fmt::format_to(ctx.out(), "Stop");
            case SCI::BAT::Gateway::SMAOperationStatus::Start:
                return fmt::format_to(ctx.out(), "Start");
            case SCI::BAT::Gateway::SMAOperationStatus::Undefined:
            default:
                return fmt::format_to(ctx.out(), "Information not available");
        }
    }
};

template <> struct fmt::formatter<SCI::BAT::Gateway::SMABatteryType>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const SCI::BAT::Gateway::SMABatteryType& lhs, FormatContext& ctx) const -> decltype(ctx.out())
    {
        switch (lhs)
        {
            case SCI::BAT::Gateway::SMABatteryType::ValveRegulatedLeadAcid:
                return fmt::format_to(ctx.out(), "Valve Regulated Lead Acid battery (VRLA)");
            case SCI::BAT::Gateway::SMABatteryType::FloodedLeadAcid:
                return fmt::format_to(ctx.out(), "Flooded lead acid batt. (FLA)");
            case SCI::BAT::Gateway::SMABatteryType::LithiumIon:
                return fmt::format_to(ctx.out(), "Lithium-Ion (Li-Ion)");
            case SCI::BAT::Gateway::SMABatteryType::Undefined:
            default:
                return fmt::format_to(ctx.out(), "Information not available");
        }
    }
};
