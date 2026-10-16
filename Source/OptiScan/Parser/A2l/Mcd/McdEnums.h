#pragma once

#include <map>
#include <string>

namespace OptiScan::Parser::A2l::Mcd
{
    enum class McdAddressType
    {
        Direct,
        PByte,
        PWord,
        PLong,
        PLongLong,
    };

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

    enum class McdDataType
    {
        None,
        UByte,
        SByte,
        UWord,
        SWord,
        ULong,
        SLong,
        AUInt64,
        AInt64,
        Float16Ieee,
        Float32Ieee,
        Float64Ieee,
    };

    enum class McdDeposit
    {
        Unknown,
        Absolute,
        Difference,
    };

    enum class McdIndexMode
    {
        RowDir,
        ColumnDir,
    };

    /** Text to value maps used with A2lTokenReader::parseEnumValue. */
    class McdEnumText
    {
    public:
        static inline const std::map<std::string, McdAddressType> AddressType =
        {
            { "DIRECT",    McdAddressType::Direct },
            { "PBYTE",     McdAddressType::PByte },
            { "PLONG",     McdAddressType::PLong },
            { "PLONGLONG", McdAddressType::PLongLong },
            { "PWORD",     McdAddressType::PWord },
        };

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

        static inline const std::map<std::string, McdDataType> DataType =
        {
            { "A_INT64",      McdDataType::AInt64 },
            { "A_UINT64",     McdDataType::AUInt64 },
            { "FLOAT16_IEEE", McdDataType::Float16Ieee },
            { "FLOAT32_IEEE", McdDataType::Float32Ieee },
            { "FLOAT64_IEEE", McdDataType::Float64Ieee },
            { "SBYTE",        McdDataType::SByte },
            { "SLONG",        McdDataType::SLong },
            { "SWORD",        McdDataType::SWord },
            { "UBYTE",        McdDataType::UByte },
            { "ULONG",        McdDataType::ULong },
            { "UWORD",        McdDataType::UWord },
        };

        static inline const std::map<std::string, McdDeposit> Deposit =
        {
            { "ABSOLUTE",   McdDeposit::Absolute },
            { "DIFFERENCE", McdDeposit::Difference },
        };

        static inline const std::map<std::string, McdIndexMode> IndexMode =
        {
            { "COLUMN_DIR", McdIndexMode::ColumnDir },
            { "ROW_DIR",    McdIndexMode::RowDir },
        };

    private:
        McdEnumText() = default;
    };

}
