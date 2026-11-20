#pragma once

#include <optional>
#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** DATA-CONSTR element: the value range of a signal.
     *
     *  INTERNAL-CONSTRS gives the range of the raw value, PHYS-CONSTRS the range of
     *  the physical value. A DBC carries the physical range in its signal line, so
     *  both are kept and the translation decides which one applies.
     *
     *  In the reference file every DATA-CONSTR holds exactly one DATA-CONSTR-RULE
     *  (704 of each), which is why a single pair of limits is modelled rather than a
     *  list of rules. */
    struct ArDataConstr
    {
        std::optional<double> _internalLowerLimit;
        std::optional<double> _internalUpperLimit;
        std::string _path;
        std::optional<double> _physLowerLimit;
        std::optional<double> _physUpperLimit;
        std::string _shortName;
    };

}
