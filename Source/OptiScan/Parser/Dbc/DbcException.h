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

    class DbcInvalidOperationException : public std::logic_error
    {
    public:
        explicit  DbcInvalidOperationException(const std::string & message);
    };

}
