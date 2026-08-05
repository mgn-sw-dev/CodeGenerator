#pragma once

#include <istream>
#include <string>
#include <vector>

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

    class LdfScanPosition
    {
    public:
        uint64_t _char = 0;
        uint64_t _charInLine = 0;
        uint64_t _line = 0;
        /** */
        LdfScanPosition() = default;
    };

    class LdfScanChar
    {
    public:
        LdfScanPosition _position;
        char _value;
    };

    class LdfToken
    {
    public:
        LdfTokenKind _kind;
        LdfScanPosition _position;
        std::string _text;
        /** */
        LdfToken();
    };

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
        std::vector<LdfScanChar> _scanBuffer;
        std::istream * _stream;
        LdfScanPosition _streamPosition;
        LdfToken _token;

        /** */
        LdfScanChar popScanBufferFront();
        /** Get @c _scanBuffer.front() value and push to @c _token._text.
         *  Erase @c _scanBuffer.front() element. */
        void popScanBufferFrontToToken();
        /** @throws FormatException. */
        void readComment();
        /** @throws FormatException. */
        void readLineComment();
        /** */
        bool scanBufferStartsWith(const std::string & pattern) const;
        /** @return false If not identifier.
         *  @throws FormatException. */
        bool tryReadIdentifier();
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
