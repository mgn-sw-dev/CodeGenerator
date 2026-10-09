#pragma once

#include <map>
#include <string>

namespace OptiScan::Parser::A2l::Mcd
{
    enum class McdByteOrder
    {
        None,
        MsbFirst,
        MsbFirstMswLast,
        MsbLast,
        MsbLastMswFirst,
    };

    enum class McdConversionType
    {
        Identical = 0,
        Formula = 1,
        Linear = 2,
        RatFunc = 3,
        TabIntp = 4,
        TabNoIntp = 5,
        TabVerb = 6,
    };

    enum class McdDeposit
    {
        Unknown,
        Absolute,
        Difference,
    };

    /** Text to value maps used with A2lTokenReader::parseEnumValue. */
    class McdEnumText
    {
    public:
        static inline const std::map<std::string, McdByteOrder> ByteOrder =
        {
            { "MSB_FIRST",          McdByteOrder::MsbFirst },
            { "MSB_FIRST_MSW_LAST", McdByteOrder::MsbFirstMswLast },
            { "MSB_LAST",           McdByteOrder::MsbLast },
            { "MSB_LAST_MSW_FIRST", McdByteOrder::MsbLastMswFirst },
        };

        static inline const std::map<std::string, McdConversionType> ConversionType =
        {
            { "FORM",       McdConversionType::Formula },
            { "IDENTICAL",  McdConversionType::Identical },
            { "LINEAR",     McdConversionType::Linear },
            { "RAT_FUNC",   McdConversionType::RatFunc },
            { "TAB_INTP",   McdConversionType::TabIntp },
            { "TAB_NOINTP", McdConversionType::TabNoIntp },
            { "TAB_VERB",   McdConversionType::TabVerb },
        };

        static inline const std::map<std::string, McdDeposit> Deposit =
        {
            { "ABSOLUTE",   McdDeposit::Absolute },
            { "DIFFERENCE", McdDeposit::Difference },
        };

    private:
        McdEnumText() = default;
    };

}
