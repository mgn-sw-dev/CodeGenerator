#pragma once

#include <stdexcept>
#include <string>

namespace OptiScan::Parser::Dbc
{

    class DbcFormatException : public std::runtime_error
    {
    public:
        explicit  DbcFormatException(const std::string & message);
    
    };

}
