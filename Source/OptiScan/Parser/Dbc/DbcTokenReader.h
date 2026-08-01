#pragma once

#include <OptiScan/Parser/Dbc/DbcScanner.h>
#include <istream>

namespace OptiScan::Parser::Dbc
{

    class DbcTokenReader 
    {
    public:
        DbcTokenReader(std::istream * input);
        /** */
        bool hasToken() const;
        /** @throws DbcFormatException. */
        static uint32_t literalIntegerTokenTextToUInt32(const std::string & tokenText);
        /** @throws DbcFormatException. */
        static double literalRealTokenTextToDouble(const std::string & tokenText);
        /** @throws  DbcFormatException*/
        static std::string literalStringTokenTextToString(const std::string & tokenText);
        /** @throws DbcFormatException. */
        void matchKeyword(const std::string & id) const;
        /** @throws DbcFormatException. */
        void matchToken(DbcTokenKind kind) const;
        /** @throws DbcFormatException. */
        void parseFloat64(double & value);
        /** @throws DbcFormatException. */
        void parseIdentifier(std::string & value);
        /** @throws DbcFormatException. */
        void parseInt32(int32_t & value);
        /** @throws DbcFormatException. */
        void parseString(std::string & value);
        /** @throws DbcFormatException. */
        void parseUInt32(uint32_t & value);
        /** @throws DbcFormatException */
        void readNextToken();
        /** */
        const DbcScanner & scanner() const;
        /** @throws DbcInvalidOperationException if no token. */
        const DbcToken & token() const;
        /** */
        const std::vector<DbcToken> & tokenStack() const;
        /** @throws DbcInvalidOperationException. */
        void tokenStackBegin();
        /** @throws DbcInvalidOperationException. */
        void tokenStackCommit();
        /** @throws DbcInvalidOperationException. */
        void tokenStackRollback();
        /** */
        int64_t tokenStackCount() const;
        /** */
        bool tryMatchKeyword(const std::string & id) const;
        /** */
        bool tryMatchToken(DbcTokenKind kind) const;
        /** @returns null If no error.
         *  @throws DbcFormatException. */
        std::exception_ptr tryParseInt32(int32_t & value);
        /** @returns null If no error.
         *  @throws DbcFormatException. */
        std::exception_ptr tryParseUInt32(uint32_t & value);
    private:
        DbcScanner _scanner;
        std::vector<DbcToken> _tokenStack;
        int64_t _tokenStackCount;
    
    };

}
