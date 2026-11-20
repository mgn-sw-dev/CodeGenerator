#pragma once

#include <OptiScan/Parser/Arxml/Ar/ArRef.h>
#include <cstdint>
#include <optional>
#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** PDU-TO-FRAME-MAPPING: where a PDU sits inside a frame. */
    struct ArPduToFrameMapping
    {
        /** MOST-SIGNIFICANT-BYTE-FIRST or MOST-SIGNIFICANT-BYTE-LAST. */
        std::string _packingByteOrder;
        std::string _path;
        ArRef _pduRef;
        std::string _shortName;
        std::optional<uint32_t> _startPosition;
    };

}
