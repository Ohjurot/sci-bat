/*!
 * @file SMAData.h
 * @brief Structures for SMA information
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
#pragma once

#include <fmt/format.h>

#include <cstdint>

namespace SCI::BAT::Gateway
{
    /*!
     * @brief Status of the sma inverter
    */
    enum class SMAStatus
    {
        /*! Status is undefined */
        Undefined = 0,
        /*! Inverter is in error state */
        Error = 35, 
        /*! Inverter is off */
        Off = 303, 
        /*! Inverter is on */
        Ok = 307,
        /*! Inverter has pending warnings */
        Warning = 455,
    };

    /*!
     * @brief Status of the battery
    */
    enum class SMABatteryStatus
    {
        /*! Battery status is undefined */
        Undefined = 0,
        /*! Battery is in the own consumption area */
        OwnConsumptionArea = 2614, 
        /*! Battery is in the state of charge conservation area */
        StateOfChargeConservationArea = 2615,
        /*! Battery is in the backup power area */
        BackupPowerArea = 2616, 
        /*! Battery is in the deep discharge protection area */
        DeepDischargeProtectionArea = 2617,
        /*! Battery is deep discharged */
        DeepDischargeArea = 2618,
    };

    /*!
     * @brief Status of the operation
    */
    enum class SMAOperationStatus
    {
        /*! The operation status is undefined */
        Undefined = 0,
        /*! Operation is stoped */
        Stop = 381,
        /*! Operation is stated */
        Start = 1467,
    };

    /*!
     * @brief Type of the battery
    */
    enum class SMABatteryType
    {
        /*! Battery type is undefined */
        Undefined = 0,
        /*! Battery type is valve regulated lead acid */
        ValveRegulatedLeadAcid = 1782,
        /*! Battery type is flooded lead acid */
        FloodedLeadAcid = 1783,
        /*! Battery type is LithiumIon */
        LithiumIon = 1785,
    };

    /*!
     * @brief SMA Input control data
    */
    struct SMAInData
    {
        /*! Current inverter status */
        SMAStatus status = SMAStatus::Undefined;
        /*! Currently delivered power */
        int32_t power = 0;
        /*! Grid voltage */
        float voltage = .0f;
        /*! Grid infrequency */
        float freqenency = .0f;
        /*! Battery current */
        float batteryCurrent = .0f;
        /*! Battery charge */
        int8_t batteryCharge = 0;
        /*! Battery capacity */
        int8_t batteryCapacity = 0;
        /*! Battery temperature */
        float batteryTemperature = .0f;
        /*! Battery voltage */
        float batteryVoltage = .0f;
        /*! Time until full charge */
        uint32_t timeUntilFullCharge = -1;
        /*! Time until full discharge */
        uint32_t timeUntilFullDischarge = -1;
        /*! Current battery status */
        SMABatteryStatus batteryStatus = SMABatteryStatus::Undefined;
        /*! Current operation status */
        SMAOperationStatus operationStaus = SMAOperationStatus::Undefined;
        /*! Battery type */
        SMABatteryType batteryType = SMABatteryType::Undefined;
        /*! Serial number of inverter */
        uint32_t serialNumber = -1;
    };

    /*!
     * @brief SMA Output control data
    */
    struct SMAOutData
    {
        /*! Indicated that power should be controlled */
        bool enablePowerControle = false;
        /*! Desired power */
        int32_t power = 0;
    };

    /*!
     * @brief Converts from 32-Bit fix point to floating point
     * @param value Input 32-Bit values
     * @param fixBits Number of fix bits
     * @return IEEE floating point representation
    */
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
