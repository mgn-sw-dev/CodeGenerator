#pragma once

#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
    /** A reference to another identifiable element.
     *
     *  ARXML does not nest, it refers: an element points at another one by its path.
     *  Because the pull parser only moves forward, the target cannot be looked up
     *  while reading. The path is therefore kept as written and resolved against
     *  ArxmlDatabase::_pathIndex once the file has been read.
     *
     *  @c _dest holds the DEST attribute, which names the expected element type. It
     *  allows a resolved reference to be checked instead of trusted. */
    struct ArRef
    {
        std::string _dest;
        std::string _path;
    };

}
