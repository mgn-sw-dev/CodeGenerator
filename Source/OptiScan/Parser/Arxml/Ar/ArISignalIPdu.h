#pragma once

#include <OptiScan/Parser/Arxml/Ar/ArDescription.h>
#include <OptiScan/Parser/Arxml/Ar/ArISignalToIPduMapping.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace OptiScan::Parser::Arxml::Ar
{
    /** I-SIGNAL-I-PDU: the payload that a frame carries.
     *
     *  This level has no counterpart in a DBC, which folds frame and PDU into one
     *  message. It is kept because it is the only way from a frame to its signals,
     *  and because the cycle time hangs here.
     *
     *  The mappings are held inline rather than in a list of their own: nothing in the
     *  file refers to a mapping by path, so there is no need to address one. */
    struct ArISignalIPdu
    {
        /** TIME-PERIOD of a CYCLIC-TIMING, in seconds. Absent for an event driven PDU. */
        std::optional<double> _cycleTime_s;
        std::optional<ArDescription> _description;
        uint32_t _length = 0;
        std::vector<ArISignalToIPduMapping> _mappings;
        std::string _path;
        std::string _shortName;
        /** TIME-OFFSET of a CYCLIC-TIMING, in seconds. */
        std::optional<double> _timeOffset_s;
        std::optional<uint32_t> _unusedBitPattern;
    };

}
