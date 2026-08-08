#pragma once

#include <OptiScan/Parser/CharReader.h>
#include <OptiScan/Parser/Token.h>
#include <istream>

namespace OptiScan::Parser::A2l
{

    enum class A2lTokenKind : int
    {
        None = 0,
        Asterisk,
        Comma,
        CompatMinus,
        Dot,
        EqualSign,
        EscapeBegin,
        EscapeEnd,
        /** first form (dot, optional exponent):  [+-]?{digit}*"."{digit}+([Ee][+-]?{digit}+)?
         *  second form (no dot, exponent): [+-]?{digit}+[Ee][+-]?{digit}+ */
        FloatLiteral,
        /** "0"[xX]{hex-digit}+ */
        HexLiteral,
        Identifier,
        /** [+-]?{digit}+ */
        IntegerLiteral,
        LeftCurlyBracket,
        LeftParenthesis,
        LeftSquareBracket,
        RightCurlyBracket,
        RightParenthesis,
        RightSquareBracket,
        Semicolon,
        StringLiteral,
    };

    using A2lToken = Token<A2lTokenKind>;

    struct A2lScannerCompatStyles
    {
        bool _allowLineEndInStringLiteral = false;
        bool _allowCompatMinus = false;
    };

    class A2lScanner 
    {
    public:
        static char const CharBackSlash;
        static char const CharCarriageReturn;
        static char const CharFormFeed;
        static char const CharHorizontalTabulation;
        static char const CharLineFeed;
        static char const CharQuotationMark;
        static char const CharSlash;
        static char const CharSpace;
        static char const CharVerticalTabulation;
        static std::string const StringCommentEnd;
        static std::string const StringCommentStart;
        static std::string const StringEscapeBegin;
        static std::string const StringEscapeEnd;
        static std::string const StringHexLowerStart;
        static std::string const StringHexUpperStart;
        static std::string const StringLineCommentStart;
        
        A2lScanner(std::istream * input);
        /** */
        void getCompatStyles(A2lScannerCompatStyles & compatStyles) const;
        /** */
        static bool isCharLineEnd(const char & c);
        /** */
        static bool isCharWhiteSpace(const char & c);
        /** @throws FormatException. */
        void scanNext();
        /** */
        void setCompatStyles(const A2lScannerCompatStyles & compatStyles);
        /** */
        const A2lToken & token() const;
    private:
        A2lScannerCompatStyles _compatStyles;
        CharReader _reader;
        A2lToken _token;

        /** @throws FormatException. */
        void readComment();
        /** @throws FormatException. */
        void readLineComment();

    };

}
