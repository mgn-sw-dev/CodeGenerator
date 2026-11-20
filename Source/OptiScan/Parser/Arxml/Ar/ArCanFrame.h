#pragma once

#include <OptiScan/Parser/Arxml/Ar/ArDescription.h>
#include <OptiScan/Parser/Arxml/Ar/ArPduToFrameMapping.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace OptiScan::Parser::Arxml::Ar
{
    /** CAN-FRAME: the frame itself, without its identifier.
     *
     *  The CAN identifier is not part of this element. A frame describes only what it
     *  carries and how long it is; where and with which identifier it is sent belongs
     *  to the CAN-FRAME-TRIGGERING. A DBC message is the two of them together. */
    struct ArCanFrame
    {
        std::optional<ArDescription> _description;
        /** Length in bytes, the DLC of a DBC message. */
        uint32_t _frameLength = 0;
        std::vector<ArPduToFrameMapping> _mappings;
        std::string _path;
        std::string _shortName;
    };

}
