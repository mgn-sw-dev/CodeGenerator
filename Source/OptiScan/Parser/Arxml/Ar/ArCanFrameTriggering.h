#pragma once

#include <OptiScan/Parser/Arxml/Ar/ArRef.h>
#include <cstdint>
#include <optional>
#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** CAN-FRAME-TRIGGERING: a frame as it appears on one channel.
     *
     *  Holds the CAN identifier and the addressing mode. Together with the referenced
     *  CAN-FRAME this is what a DBC calls a message.
     *
     *  @c _identifier is the plain identifier. A DBC marks an extended identifier by
     *  setting bit 31 of its message id, so the translation has to add that bit when
     *  @c _canAddressingMode is EXTENDED. */
    struct ArCanFrameTriggering
    {
        /** STANDARD or EXTENDED. */
        std::string _canAddressingMode;
        std::string _canFrameRxBehavior;
        std::string _canFrameTxBehavior;
        ArRef _frameRef;
        std::optional<uint32_t> _identifier;
        std::string _path;
        std::string _shortName;
    };

}
