#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** CAN-CLUSTER: one CAN network.
     *
     *  The baud rates sit below CAN-CLUSTER-VARIANTS and CAN-CLUSTER-CONDITIONAL, which
     *  are pure wrappers and do not show up here. */
    struct ArCanCluster
    {
        std::optional<uint32_t> _baudrate_bps;
        std::optional<uint32_t> _canFdBaudrate_bps;
        std::string _path;
        std::string _shortName;
        std::optional<std::string> _protocolName;
    };

}
