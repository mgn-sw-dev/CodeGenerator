#pragma once

#include <string>
#include <vector>

namespace OptiScan::Parser::Dbc
{
    class DbcDatabase
    {
    public:
        std::vector<std::string> _newSymbols;
        std::string _version;
    };

}
