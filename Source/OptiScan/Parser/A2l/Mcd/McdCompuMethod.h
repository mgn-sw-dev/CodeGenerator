#pragma once

#include <OptiScan/Parser/A2l/Mcd/McdEnums.h>
#include <OptiScan/Parser/A2l/Mcd/McdIdent.h>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace OptiScan::Parser::A2l::Mcd
{
    /** COEFFS of a COMPU_METHOD: rational function coefficients. */
    struct McdCoeffs
    {
        double _a;
        double _b;
        double _c;
        double _d;
        double _e;
        double _f;
        /** */
        McdCoeffs();
    };

    /** COEFFS_LINEAR of a COMPU_METHOD. */
    struct McdCoeffsLinear
    {
        double _a;
        double _b;
        /** */
        McdCoeffsLinear();
    };

    /** FORMULA of a COMPU_METHOD. @c _g is the optional inverse formula. */
    struct McdFormula
    {
        std::string _f;
        std::string _g;
    };

    struct McdCompuMethod
    {
        std::optional<McdCoeffs> _coeffs;
        std::optional<McdCoeffsLinear> _coeffsLinear;
        McdIdent _compuTabRef;
        McdConversionType _conversionType;
        std::string _format;
        std::optional<McdFormula> _formula;
        std::string _longIdentifier;
        McdIdent _name;
        std::string _physicalUnit;
        McdIdent _refUnit;
        McdIdent _statusStringRef;
        /** */
        McdCompuMethod();
    };

    /** COMPU_TAB. @c _defaultValue holds a string for DEFAULT_VALUE and a double for
     *  DEFAULT_VALUE_NUMERIC. */
    struct McdCompuTab
    {
        McdConversionType _conversionType;
        std::optional<std::variant<double, std::string>> _defaultValue;
        std::string _longIdentifier;
        McdIdent _name;
        std::vector<std::tuple<double, double>> _valuePairs;
        /** */
        McdCompuTab();
    };

    struct McdCompuVTab
    {
        McdConversionType _conversionType;
        std::string _defaultValue;
        std::string _longIdentifier;
        McdIdent _name;
        std::vector<std::tuple<double, std::string>> _valuePairs;
        /** */
        McdCompuVTab();
    };

    /** COMPU_VTAB_RANGE has no conversion type. */
    struct McdCompuVTabRange
    {
        std::string _defaultValue;
        std::string _longIdentifier;
        McdIdent _name;
        std::vector<std::tuple<double, double, std::string>> _valueTriples;
    };

}
