#pragma once

#include <OptiScan/Parser/A2l/A2lScanner.h>
#include <cstdint>
#include <deque>
#include <istream>
#include <map>
#include <string>
#include <unordered_set>

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
        /** Expects "/begin <keyword>" and consumes both tokens.
         *  @throws FormatException. */
        void parseBlockBegin(const std::string & keyword);
        /** Expects "/begin <identifier>", returns the identifier and consumes both tokens.
         *  @throws FormatException. */
        void parseBlockBeginAny(std::string & keyword);
        /** Expects "/end <keyword>" and consumes both tokens.
         *  @throws FormatException. */
        void parseBlockEnd(const std::string & keyword);
        /** Look up the current identifier token in @p map and consume it.
         *  @param name: value description used in the error message.
         *  @throws FormatException. */
        template <typename E>
        void parseEnumValue(const std::map<std::string, E> & map, const std::string & name, E & value);
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
        /** Skip a complete "/begin <identifier> ... /end <identifier>" block of unknown structure.
         *  @throws FormatException. */
        void parseUnknownBlock();
        /** Skip the content of a block of unknown structure. Nested blocks and bracket pairs
         *  are tracked, so the method stops on the EscapeEnd token closing the current block.
         *  That token is not consumed; use @ref parseBlockEnd afterwards.
         *  @throws FormatException. */
        void parseUnknownBlockContent();
        /** @throws FormatException */
        void readNextToken();
        /** */
        const A2lScanner & scanner() const;
        /** Build a message with the current token position and throw a FormatException.
         *  @param context: "Class::method" of the caller.
         *  @throws FormatException. */
        [[noreturn]] void throwFormatError(const std::string & context, const std::string & message) const;
        /** */
        const A2lToken & token() const;
        /** Readable name of @p kind, used in error messages. */
        static std::string tokenKindToText(A2lTokenKind kind);
        /** */
        const std::deque<A2lToken> & tokenStack() const;
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
        /** Initial capacity of the block stack used by @ref parseUnknownBlockContent. */
        static const size_t UnknownBlockStackReserve;

        A2lScanner _scanner;
        std::deque<A2lToken> _tokenStack;
        int64_t _tokenStackCount;

        /** Number of buffered tokens as a signed value, for comparison with _tokenStackCount. */
        int64_t tokenStackSize() const;
    };

}

#include <OptiScan/Parser/A2l/A2lTokenReader.inl.h>
