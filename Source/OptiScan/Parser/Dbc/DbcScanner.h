#pragma once

#include <fstream>
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

    class DbcScanner 
    {
    public:
        DbcScanner(std::istream * input);
        /** @throw invalide_argiment if unknown token. */
        void scanNext();
        /** @return DbcScanPosition. */
        const DbcScanPosition & streamPosition() const;
        /** @return DbcToken. */
        const DbcToken & token() const;

    private:
        std::vector<DbcScanChar> _scanBuffer;
        std::istream * _stream;
        DbcScanPosition _streamPosition;
        DbcToken _token;
    };

}
