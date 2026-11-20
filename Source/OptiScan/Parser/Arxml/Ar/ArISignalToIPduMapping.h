#pragma once

#include <OptiScan/Parser/Arxml/Ar/ArRef.h>
#include <cstdint>
#include <optional>
#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** I-SIGNAL-TO-I-PDU-MAPPING: where a signal sits inside a PDU.
     *
     *  This is the element a DBC carries as the start bit and byte order of a signal.
     *
     *  A mapping points either at a single signal through @c _iSignalRef or at a group
     *  through @c _iSignalGroupRef. Group mappings have no start position of their own,
     *  which is why @c _startPosition is optional. */
    struct ArISignalToIPduMapping
    {
        ArRef _iSignalGroupRef;
        ArRef _iSignalRef;
        /** MOST-SIGNIFICANT-BYTE-FIRST or MOST-SIGNIFICANT-BYTE-LAST. */
        std::string _packingByteOrder;
        std::string _path;
        std::string _shortName;
        std::optional<uint32_t> _startPosition;
        std::string _transferProperty;
    };

}
