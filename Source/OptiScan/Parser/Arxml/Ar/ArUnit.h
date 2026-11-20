#pragma once

#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** UNIT element. @c _displayName is what a DBC carries as the unit of a signal. */
    struct ArUnit
    {
        std::string _displayName;
        std::string _path;
        std::string _shortName;
    };

}
