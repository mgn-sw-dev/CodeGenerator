#pragma once

#include <string>
#include <variant>
#include <vector>

namespace OptiScan::Parser::A2l
{
    using McdArrayIndex = std::variant<int64_t, std::string>;

    class McdIdentPart
    {
    public:
        std::vector<McdArrayIndex> _arrayIndexList;
        std::string _name;
        /** */
        McdIdentPart(const std::string & name = std::string(), const std::vector<McdArrayIndex> & arrayIndexList = {});
        /** */
        std::string toString() const;
    };

    class McdIdent
    {
    public:
        std::vector<McdIdentPart> _parts;
        /** */
        McdIdent(const std::vector<McdIdentPart> & parts = std::vector<McdIdentPart>() );
        /** */
        std::string toString() const;
        /** */
        bool operator == (const McdIdent & other) const;

    };
}
