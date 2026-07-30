#pragma once

#include <string>
#include <vector>

namespace OptiScan::Parser::Dbc
{
    class DbcStringEscapeItem
    {
    public:
        char _escapeMarker;
        char _escapedChar;
        bool _isOptional;
    };

    class DbcFormat 
    {
    public:
        static std::vector<DbcStringEscapeItem> const StringEscapeItems;
        static char const StringEscapeStart;
    };

}
