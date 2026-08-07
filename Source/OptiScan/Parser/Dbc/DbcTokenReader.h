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
        /** @throws  FormatException*/
        static std::string literalStringTokenTextToString(const std::string & tokenText);
        /** @throws FormatException. */
        void matchKeyword(const std::string & id) const;
        /** @throws FormatException. */
        void matchToken(DbcTokenKind kind) const;
        /** @throws FormatException. */
        void parseFloat64(double & value);
        /** @throws FormatException. */
        void parseIdentifier(std::string & value);
        /** @throws FormatException. */
        void parseInt32(int32_t & value);
        /** @throws FormatException. */
        void parseString(std::string & value);
        /** @throws FormatException. */
        void parseUInt32(uint32_t & value);
        /** @throws FormatException */
        void readNextToken();
        /** */
        const DbcScanner & scanner() const;
        /** @throws InvalidOperationException if no token. */
        const DbcToken & token() const;
        /** */
        const std::vector<DbcToken> & tokenStack() const;
        /** @throws InvalidOperationException. */
        void tokenStackBegin();
        /** @throws InvalidOperationException. */
        void tokenStackCommit();
        /** @throws InvalidOperationException. */
        void tokenStackRollback();
        /** */
        int64_t tokenStackCount() const;
        /** */
        bool tryMatchKeyword(const std::string & id) const;
        /** */
        bool tryMatchToken(DbcTokenKind kind) const;
        /** @returns null If no error.
         *  @throws FormatException. */
        std::exception_ptr tryParseInt32(int32_t & value);
        /** @returns null If no error.
         *  @throws FormatException. */
        std::exception_ptr tryParseUInt32(uint32_t & value);
    private:
        DbcScanner _scanner;
        std::vector<DbcToken> _tokenStack;
        int64_t _tokenStackCount;
    
    };

}
