#pragma once

#include <string>
#include <utility>

namespace OptiScan::Parser
{
    class TokenReaderUtils 
    {
    public:
        /** @throws FormatException. */
        static uint32_t literalHexIntegerTokenTextToUInt32(const std::string & tokenText);
        /** @throws FormatException. */
        static uint32_t literalIntegerTokenTextToUInt32(const std::string & tokenText);
        /** @throws FormatException. */
        static double literalRealTokenTextToDouble(const std::string & tokenText);
        /**  @throws FormatException. */
        static std::string literalStringTokenTextToString(const std::string & tokenText);
        /** @throws FormatException. */
        static std::pair<uint8_t, uint8_t> literalStringTokenTextToVersionPair(const std::string_view & tokenText);
    private:
        TokenReaderUtils() = default;
    };

}
