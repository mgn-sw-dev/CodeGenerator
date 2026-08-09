#pragma once

#include <OptiScan/Parser/CharReader.h>
#include <OptiScan/Parser/Token.h>
#include <istream>

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

    using DbcToken = Token<DbcTokenKind>;

    class DbcScanner 
    {
    public:
        DbcScanner(std::istream * input);
        /** */
        static bool isCharWhiteSpace(const char & c);
        /** @throw FormatException if unknown token. */
        void scanNext();
        /** @return DbcToken. */
        const DbcToken & token() const;

    private:
        CharReader _reader;
        DbcToken _token;

        /** @return false If not end of line.
         *  @throws FormatException. */
        bool readEndOfLine();
        /** @return false If not literal integer or literal real.
         *  @throws FormatException. */
        bool readLiteralIntegerOrReal();
        /** @return false If not literal string.
         *  @throws FormatException. */
        bool readLiteralString();
        /** @return false If not operator.
         *  @throws FormatException. */
        bool readOperator();
    };

}
