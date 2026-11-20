#pragma once

#include <OptiScan/Parser/Arxml/Ar/ArDescription.h>
#include <OptiScan/Parser/Arxml/Ar/ArRef.h>
#include <cstdint>
#include <optional>
#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** I-SIGNAL element: the signal as it appears on the bus.
     *
     *  @c _length is the bit length. The conversion and the value range are reached
     *  through @c _systemSignalRef, the sign through @c _baseTypeRef. Where the signal
     *  sits inside a PDU is not part of this element; that is the
     *  I-SIGNAL-TO-I-PDU-MAPPING. */
    struct ArISignal
    {
        ArRef _baseTypeRef;
        std::optional<ArDescription> _description;
        std::optional<double> _initValue;
        std::optional<double> _invalidValue;
        uint32_t _length = 0;
        std::string _path;
        std::string _shortName;
        ArRef _systemSignalRef;
    };

}
