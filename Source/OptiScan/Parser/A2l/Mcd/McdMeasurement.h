#pragma once

#include <OptiScan/Parser/A2l/Mcd/McdEnums.h>
#include <OptiScan/Parser/A2l/Mcd/McdIdent.h>
#include <OptiScan/Parser/A2l/Mcd/McdIfData.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace OptiScan::Parser::A2l::Mcd
{
    /** MAX_REFRESH of a MEASUREMENT: maximum refresh rate of the value inside the ECU. */
    struct McdMaxRefresh
    {
        /** Number of scaling units between two updates. */
        uint32_t _rate;
        /** Scaling unit according to ASAP2, for example 3 for milliseconds. */
        uint16_t _scalingUnit;
        /** */
        McdMaxRefresh();
    };

    /** SYMBOL_LINK of a MEASUREMENT: symbol name from the linker output plus an offset. */
    struct McdSymbolLink
    {
        int32_t _offset;
        std::string _symbolName;
        /** */
        McdSymbolLink();
    };

    /** Move only, because the IF_DATA contents are held by unique_ptr. */
    struct McdMeasurement
    {
        /** Conversion name used when a measurement has no COMPU_METHOD. */
        static inline const std::string ConversionNoCompuMethod = "NO_COMPU_METHOD";

        double _accuracy;
        McdAddressType _addressType;
        std::optional<uint16_t> _arraySize;
        /** Defaults to UINT64_MAX, meaning no masking. */
        uint64_t _bitMask;
        McdByteOrder _byteOrder;
        McdIdent _conversion;
        McdDataType _dataType;
        McdIdent _displayIdentifier;
        std::optional<uint32_t> _ecuAddress;
        std::optional<int16_t> _ecuAddressExtension;
        uint64_t _errorMask;
        /** %[<length>].<layout> */
        std::string _format;
        std::vector<McdIfDataPtr> _ifDatas;
        McdIndexMode _indexMode;
        bool _isDiscrete;
        bool _isWriteable;
        std::string _longIdentifier;
        double _lowerLimit;
        std::vector<uint16_t> _matrixDim;
        std::optional<McdMaxRefresh> _maxRefresh;
        std::optional<std::string> _modelLink;
        McdIdent _name;
        std::string _physicalUnit;
        uint16_t _resolution;
        std::optional<McdSymbolLink> _symbolLink;
        double _upperLimit;
        /** */
        McdMeasurement();
    };

}
