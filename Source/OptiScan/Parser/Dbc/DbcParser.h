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
        /** @throws DbcFormatException. */
        static uint32_t literalIntegerTokenTextToUInt32(const std::string & tokenText);
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
        int64_t _tokenStackCount;

        /** @throws DbcFormatException. */
        void matchKeyword(const std::string & id);
        /** @throws DbcFormatException. */
        void matchToken(DbcTokenKind kind);
        /** @throws DbcFormatException. */
        void parseBitTiming(DbcBitTiming & bitTiming);
        /** @throws DbcFormatException. */
        void parseEndOfLine();
        /** @throws DbcFormatException. */
        void parseIdentifier(std::string & value);
        /** @throws DbcFormatException. */
        void parseNewSymbols(std::vector<std::string> & symbols);
        /** @throws DbcFormatException. */
        void parseString(std::string & value);
        /** @throws DbcFormatException. */
        void parseUInt32(uint32_t & value);
        /** @throws DbcFormatException. */
        void parseVersion(std::string & version);
        /** @throws DbcFormatException */
        void readNextToken();
        /** @throws DbcInvalidOperationException. */
        void tokenStackBegin();
        /** @throws DbcInvalidOperationException. */
        void tokenStackCommit();
        /** @throws DbcInvalidOperationException. */
        void tokenStackRollback();
        /** */
        bool tryMatchKeyword(const std::string & id);
        /** */
        bool tryMatchToken(DbcTokenKind kind) const;
        /** @returns null If no error.
         *  @throws DbcFormatException. */
        std::exception_ptr tryParseUInt32(uint32_t & value);
    };

}
