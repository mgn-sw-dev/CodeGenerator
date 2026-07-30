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
        bool hasToken() const;
        /** @throws  DbcFormatException*/
        static std::string literalStringTokenTextToString(const std::string & tokenText);
        /** */
        void parse(DbcDatabase & dbcDatabase);
        /** @throws std::runtime_error if no token. */
        const DbcToken & token() const;
    private:
        // DbcParserObserver * _observer
        DbcScanner _scanner;
        std::vector<DbcToken> _tokenStack;
        size_t _tokenStackCount;

        /** @throws DbcFormatException. */
        void matchKeyword(const std::string & id);
        /** @throws DbcFormatException. */
        void matchToken(DbcTokenKind kind);
        /** @throws DbcFormatException. */
        void parseEndOfLine();
        /** @throws DbcFormatException. */
        void parseString(std::string & value);
        /** @throws DbcFormatException. */
        void parseVersion(std::string & version);
        /** */
        void readNextToken();
        /** */
        bool tryMatchKeyword(const std::string & id);
        /** */
        bool tryMatchToken(DbcTokenKind kind);
    };

}
