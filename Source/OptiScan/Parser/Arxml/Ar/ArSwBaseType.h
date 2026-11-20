#pragma once

#include <cstdint>
#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** SW-BASE-TYPE element.
     *
     *  Reached from an I-SIGNAL through BASE-TYPE-REF. @c _baseTypeEncoding tells
     *  whether a signal is signed ("2C" for two's complement), unsigned ("NONE") or a
     *  floating point value ("IEEE754") - the DBC equivalent of the sign flag. */
    struct ArSwBaseType
    {
        std::string _baseTypeEncoding;
        uint32_t _baseTypeSize = 0;
        std::string _path;
        std::string _shortName;
    };

}
