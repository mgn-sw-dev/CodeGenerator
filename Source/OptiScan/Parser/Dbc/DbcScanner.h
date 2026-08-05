#pragma once

#include <OptiScan/Parser/CharReader.h>
#include <OptiScan/Parser/Token.h>
#include <cstdint>
#include <istream>
#include <string>
#include <vector>

namespace OptiScan::Parser::Dbc
{
    enum class DbcTokenKind : int
    {
        None = 0,
        EndOfLine,
        Identifier,
        /** {DIGIT}+ */
        LiteralInteger,
        /** first form (dot, optional exponent):  {DIGIT}*"."{DIGIT}+([Ee][+-]?{DIGIT}+)?
         *  second form (no dot, exponent): {DIGIT}+[Ee][+-]?{DIGIT}+ */
        LiteralReal,
        /** Using escape sequences \\ => \, \" => ", \r => <CR>, \n => <LF>. */
        LiteralString,
        OperatorColon,
        OperatorComma,
        OperatorCommercialAt,
        OperatorLeftParenthesis,
        OperatorLeftSquareBracket,
        OperatorMinus,
        OperatorPlus,
        OperatorRightParenthesis,
        OperatorRightSquareBracket,
        OperatorSemicolon,
        OperatorVerticalLine,
    };
#if  0
    class DbcScanPosition
    {
    public:
        int64_t _char;
        int64_t _charInLine;
        int64_t _line;
        /** */
        DbcScanPosition();
    };

    class DbcScanChar
    {
    public:
        DbcScanPosition _position;
        char _value;
    };

    class DbcToken
    {
    public:
        DbcTokenKind _kind;
        DbcScanPosition _position;
        std::string _text;
        /** */
        DbcToken();
    };
#endif
    using DbcToken = Token<DbcTokenKind>;

    class DbcScanner 
    {
    public:
        DbcScanner(std::istream * input);
        /** */
        static bool isCharDecimalSeparator(const char & c);
        /** */
        static bool isCharExponentStart(const char & c);
        /** */
        static bool isCharWhiteSpace(const char & c);
        /** @throw DbcFormatException if unknown token. */
        void scanNext();
        /** @return DbcToken. */
        const DbcToken & token() const;

    private:
        CharReader _reader;
        DbcToken _token;

        /** Get @c _scanBuffer.front() value and push to @c _token._text.
         *  Erase @c _scanBuffer.front() element. */
        void popScanBufferFrontToToken();
        /** @return false If not end of line.
         *  @throws DbcFormatException. */
        bool readEndOfLine();
        /** @return false If not identifier.
         *  @throws DbcFormatException. */
        bool readIdentifier();
        /** @return false If not literal integer or literal real.
         *  @throws DbcFormatException. */
        bool readLiteralIntegerOrReal();
        /** @return false If not literal string.
         *  @throws DbcFormatException. */
        bool readLiteralString();
        /** @return false If not operator.
         *  @throws DbcFormatException. */
        bool readOperator();
    };

}
