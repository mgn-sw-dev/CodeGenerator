#pragma once

#include <OptiScan/Parser/A2l/A2lScanner.h>
#include <istream>
#include <unordered_set>
#include <vector>

namespace OptiScan::Parser::A2l
{

    class A2lTokenReader 
    {
    public:
        A2lTokenReader(std::istream * input);
        /** */
        bool hasToken() const;
        /** @throws FormatException. */
        void matchKeyword(const std::string & id) const;
        /** @throws FormatException. */
        void matchToken(A2lTokenKind kind) const;
        /** */
        static void trackOneTimeKeyword(std::unordered_set<std::string> & oneTimeKeywords, const std::string & keyword);
        /** @throws FormatException. */
        void parseFloat64(double & value);
        /** @throws FormatException. */
        void parseInt16(int16_t & value);
        /** @throws FormatException. */
        void parseInt32(int32_t & value);
        /** @throws FormatException. */
        void parseInt64(int64_t & value);
        /** @throws FormatException. */
        void parseString(std::string & value);
        /** @throws FormatException. */
        void parseUInt8(uint8_t & value);
        /** @throws FormatException. */
        void parseUInt16(uint16_t & value);
        /** @throws FormatException. */
        void parseUInt32(uint32_t & value);
        /** @throws FormatException. */
        void parseUInt64(uint64_t & value);
        /** @throws FormatException */
        void readNextToken();
        /** */
        const A2lScanner & scanner() const;
        /** */
        const A2lToken & token() const;
        /** */
        const std::vector<A2lToken> & tokenStack() const;
        /** @throws InvalidOperationException. */
        void tokenStackBegin();
        /** @throws InvalidOperationException. */
        void tokenStackCommit();
        /** */
        int64_t tokenStackCount() const;
        /** @throws InvalidOperationException. */
        void tokenStackRollback();
        /** */
        bool tryMatchKeyword(const std::string & id) const;
        /** */
        bool tryMatchToken(A2lTokenKind kind) const;
        /** */
        bool tryMatchToken(A2lTokenKind kind, std::exception_ptr & error) const;
        /** @throws InvalidOperationException.
         *  @throws FormatException. */
        bool tryParseUInt16(uint16_t & value, std::exception_ptr & error);
        /**@throws FormatException.*/
        bool tryTokenStackBlockBegin(const std::string & keyword);
        /**@throws FormatException.*/
        bool tryTokenStackBlockBegin(const std::string & keyword,std::exception_ptr & error);
        /** @throws FormatException.*/
        bool tryTokenStackBlockBeginAny(std::string & keyword);
        /** @throws FormatException.*/
        bool tryTokenStackBlockBeginAny(std::string & keyword, std::exception_ptr & error);
    private:
        A2lScanner _scanner;
        std::vector<A2lToken> _tokenStack;
        int64_t _tokenStackCount;
    };

}
