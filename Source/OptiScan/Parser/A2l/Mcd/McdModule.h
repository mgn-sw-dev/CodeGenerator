#pragma once

#include <OptiScan/Parser/A2l/Mcd/McdEnums.h>
#include <OptiScan/Parser/A2l/Mcd/McdIdent.h>
#include <OptiScan/Parser/A2l/Mcd/McdIfData.h>
#include <cstdint>
#include <string>
#include <vector>

namespace OptiScan::Parser::A2l::Mcd
{
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

    /** Move only, because the IF_DATA contents are held by unique_ptr. */
    struct McdModule 
    {
        McdModuleCommon _common;
        // M3: std::vector<McdCompuMethod> _compuMethods;
        // M3: std::vector<McdCompuTab> _compuTabs;
        // M3: std::vector<McdCompuVTabRange> _compuVTabRanges;
        // M3: std::vector<McdCompuVTab> _compuVTabs;
        std::vector<McdIfDataPtr> _ifDatas;
        std::string _longIdentifier;
        // M4: std::vector<McdMeasurement> _measurements;
        McdIdent _name;
        // M3: McdModuleParameters _parameters;
    };

}
