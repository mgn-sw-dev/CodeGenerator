#pragma once

#include <OptiScan/Parser/A2l/IfData/A2lIfDataParser.h>
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpIfData.h>

namespace OptiScan::Parser::A2l::IfData::Xcp
{
    /** Parser for the classic "IF_DATA XCP". */
    class XcpIfDataParser : public IA2lIfDataParser
    {
    public:
        /** */
        bool canParse(const Mcd::McdIdent & name) const override;
        /** @throws FormatException. */
        Mcd::McdIfDataPtr parseContent(A2lTokenReader & reader) override;

    private:
        /** @throws FormatException. */
        static void parseXcpOnCanContent(A2lTokenReader & reader, XcpXcpOnCan & xcpOnCan);
    };

    /** Parser for "IF_DATA XCPplus". */
    class XcpPlusIfDataParser : public IA2lIfDataParser
    {
    public:
        /** */
        bool canParse(const Mcd::McdIdent & name) const override;
        /** @throws FormatException. */
        Mcd::McdIfDataPtr parseContent(A2lTokenReader & reader) override;

    private:
        /** @throws FormatException. */
        static void parseXcpOnCanContent(A2lTokenReader & reader, XcpPlusXcpOnCan & xcpOnCan);
    };

}
