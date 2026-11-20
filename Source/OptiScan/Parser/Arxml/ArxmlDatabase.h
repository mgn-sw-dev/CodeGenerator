#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace OptiScan::Parser::Arxml
{
    /** One identifiable element of the ARXML, that is one element carrying a
     *  SHORT-NAME. Its path is the key under which it is held in the database.
     *
     *  Starting with N3 the entry will also point at the parsed model object; for now
     *  it records what kind of element sits at that path, which is enough to check a
     *  reference against the DEST attribute it carries. */
    struct ArxmlIdentifiable
    {
        /** Name of the XML element, for example "I-SIGNAL". */
        std::string _elementType;
        uint64_t _lineNumber = 0;
    };

    /** Content of one ARXML file.
     *
     *  The path index is the backbone of the two phase approach (plan B4): the parser
     *  cannot follow a reference while reading, because the pull parser only moves
     *  forward. References are therefore kept as paths and resolved against this index
     *  once the file has been read. */
    class ArxmlDatabase
    {
    public:
        /** Paths that occur more than once. AUTOSAR requires them to be unique, so a
         *  non empty list points at a broken file or at a gap in the parser. */
        std::vector<std::string> _duplicatePaths;
        /** AUTOSAR path, for example "/Signals/BACKBONE/EngSpd", to element. */
        std::map<std::string, ArxmlIdentifiable> _pathIndex;
    };

}
