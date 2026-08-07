#pragma once

#include <OptiScan/Parser/Ldf/LdfScanner.h>
#include <istream>

namespace OptiScan::Parser::Ldf
{

    class LdfTokenReader 
    {
    public:
        LdfTokenReader(std::istream * input);
        /** */
        bool hasToken() const;
        /** @throws FormatException. */
        static uint32_t literalHexIntegerTokenTextToUInt32(const std::string & tokenText);
        /** @throws FormatException. */
        static uint32_t literalIntegerTokenTextToUInt32(const std::string & tokenText);
        /** @throws FormatException. */
        static double literalRealTokenTextToDouble(const std::string & tokenText);
        /**  @throws FormatException. */
        static std::string literalStringTokenTextToString(const std::string & tokenText);
        /** @throws FormatException. */
        void matchKeyword(const std::string & id) const;
        /** @throws FormatException. */
        void matchKeywordAndRead(const std::string & id);
        /** @throws FormatException. */
        void matchToken(LdfTokenKind kind) const;
        /** @throws FormatException. */
        void matchTokenAndRead(LdfTokenKind kind);
        /** @throws FormatException. */
        void parseFloat64(double & value);
        /** @throws FormatException. */
        void parseFloat64_ms(double & value_ms);
        /** @throws FormatException. */
        void parseIdentifier(std::string & value);
        /** @throws FormatException. */
        void parseUInt8(uint8_t & value);
        /** @throws FormatException. */
        void parseUInt16(uint16_t & value);
        /** @throws FormatException. */
        void parseUInt32(uint32_t & value);
        /** @throws FormatException. */
        void parseString(std::string & value);
        /** @throws FormatException */
        void readNextToken();
        /** @throws InvalidOperationException if no token. */
        const LdfToken & token() const;
        /** */
        bool tryMatchKeyword(const std::string & id) const;
        /** */
        bool tryMatchToken(LdfTokenKind kind) const;


    private:
        LdfScanner _scanner;
        std::vector<LdfToken> _tokenStack;
        int64_t _tokenStackCount;
    };

}
