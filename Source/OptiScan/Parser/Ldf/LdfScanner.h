#pragma once

#include <OptiScan/Parser/CharReader.h>
#include <OptiScan/Parser/Token.h>
#include <istream>

namespace OptiScan::Parser::Ldf
{
    enum class LdfTokenKind : int
    {
        None = 0,
        Identifier,
        /** "0x"{hex-digit}+ */
        LiteralHexInteger,
        /** [+-]?{digit}+ */
        LiteralInteger,
        /** [+-]?{digit}+"."{digit}+ */
        LiteralReal,
        LiteralString,
        OperatorColon,
        OperatorComma,
        OperatorEqualSign,
        OperatorLeftCurlyBracket,
        OperatorRightCurlyBracket,
        OperatorSemicolon,
    };

    using LdfToken = Token<LdfTokenKind>;

    class LdfScanner 
    {
    public:
        static std::string const StringCommentEnd;
        static std::string const StringCommentStart;
        static std::string const StringLineCommentStart;

        LdfScanner(std::istream * input);
        /** */
        static bool isCharHexDigit(const char & c);
        /** */
        static bool isCharLineEnd(const char & c);
        /** */
        static bool isCharWhiteSpace(const char & c);
        /** @throws FormatException. */
        void scanNext();
        /** */
        const LdfToken & token() const;
    private:
        CharReader _reader;
        LdfToken _token;

        /** @throws FormatException. */
        void readComment();
        /** @throws FormatException. */
        void readLineComment();
        /** @return false If not literal hex integer.
         *  @throws FormatException. */
        bool tryReadLiteralHexInteger();
        /** @return false If not literal integer or literal real.
         *  @throws FormatException. */
        bool tryReadLiteralIntegerOrReal();
        /** @return false If not literal string.
         *  @throws FormatException. */
        bool tryReadLiteralString();
        /** @return false If not operator.
         *  @throws FormatException. */
        bool tryReadOperator();
    };

}
