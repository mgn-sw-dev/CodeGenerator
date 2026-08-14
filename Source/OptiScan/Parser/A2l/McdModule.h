#pragma once

#include <OptiScan/Parser/A2l/McdIdent.h>
#include <string>
#include <variant>
#include <vector>
#include <optional>

namespace OptiScan::Parser::A2l
{
    enum class McdByteOrder
    {
        None,
        MsbFirst,
        MsbFirstMswLast,
        MsbLast,
        MsbLastMswFirst,
    };

    enum class McdDeposit
    {
        Unknown,
        Absolute,
        Difference,
    };

    struct McdModuleCommon
    {
        uint16_t _alignmentByte;
        uint16_t _alignmentFloat16Ieee;
        uint16_t _alignmentFloat32Ieee;
        uint16_t _alignmentFloat64Ieee;
        uint16_t _alignmentInt64;
        uint16_t _alignmentLong;
        uint16_t _alignmentWord;
        McdByteOrder _byteOrder;
        std::string _comment;
        uint16_t _dataSize;
        McdDeposit _deposit;
        /** */
        McdModuleCommon();
    };
    
    struct McdModule 
    {
        McdModuleCommon _common;
        // std::vector<McdCompuMethod> _compuMethods;
        // std::vector<McdCompuTab> _compuTabs;
        // std::vector<McdCompuVTabRange> _compuVTabRanges;
        // std::vector<McdCompuVTab> _compuVTabs;
        /** polymorph - need shared_ptr */
        // std::vector<std::shared_ptr<McdIfData>> _ifDatas;
        std::string _longIdentifier;
        // std::vector<McdMeasurement> _measurements;
        McdIdent _name;
        // McdModuleParameters _parameters;
    };

}
