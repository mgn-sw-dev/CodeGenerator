#pragma once

#include <OptiScan/Parser/Dbc/DbcDatabase.h>
#include <OptiScan/Parser/Dbc/DbcScanner.h>
#include <fstream>
#include <vector>

namespace OptiScan::Parser::Dbc
{
    class DbcParser 
    {
    public:
        DbcParser(std::istream * input);
        /** */
        void parse(DbcDatabase & dbcDatabase);
    private:
        // DbcParserObserver * _observer
        DbcScanner _scanner;
        std::vector<DbcToken> _tokenStack;
        size_t _tokenStackCount;

        void readNextToken();
    };

}
