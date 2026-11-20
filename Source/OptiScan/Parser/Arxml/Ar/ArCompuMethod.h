#pragma once

#include <OptiScan/Parser/Arxml/Ar/ArRef.h>
#include <optional>
#include <string>
#include <vector>

namespace OptiScan::Parser::Arxml::Ar
{
    /** One COMPU-SCALE of a COMPU-METHOD.
     *
     *  A scale is either a text table entry, then @c _textValue holds the text of
     *  COMPU-CONST/VT, or a linear conversion, then @c _numerator and @c _denominator
     *  hold the coefficients of COMPU-RATIONAL-COEFFS.
     *
     *  The coefficients are kept exactly as written instead of being folded into
     *  factor and offset: AUTOSAR orders them ascending, so the numerator is
     *  (offset, factor) and the denominator usually (1). Folding is the job of the
     *  translation into the DBC world, not of the parser. */
    struct ArCompuScale
    {
        std::vector<double> _denominator;
        std::optional<double> _lowerLimit;
        std::vector<double> _numerator;
        std::string _shortLabel;
        std::string _symbol;
        std::optional<std::string> _textValue;
        std::optional<double> _upperLimit;
    };

    /** COMPU-METHOD element.
     *
     *  @c _category is kept as written, for example "TEXTTABLE", "LINEAR" or
     *  "IDENTICAL". It is not mapped to an enumeration because the set of categories
     *  used in the field is not known yet, and an unknown value should not make the
     *  file unparsable. */
    struct ArCompuMethod
    {
        std::string _category;
        std::string _path;
        std::vector<ArCompuScale> _scales;
        std::string _shortName;
        ArRef _unitRef;
    };

}
