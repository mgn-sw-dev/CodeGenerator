#pragma once

#include <OptiScan/Parser/Arxml/Ar/ArDescription.h>
#include <OptiScan/Parser/Arxml/Ar/ArRef.h>
#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** SYSTEM-SIGNAL element: the signal as the network describes it.
     *
     *  The conversion and the value range hang here, not on the I-SIGNAL, which only
     *  describes how the value sits on the bus. */
    struct ArSystemSignal
    {
        ArRef _compuMethodRef;
        ArRef _dataConstrRef;
        std::optional<ArDescription> _description;
        bool _isDynamicLength = false;
        std::string _path;
        std::string _shortName;
    };

}
