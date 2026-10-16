#pragma once

#include <OptiScan/Parser/A2l/A2lTokenReader.h>
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpIfData.h>
#include <string>
#include <unordered_set>

namespace OptiScan::Parser::A2l::IfData::Xcp
{
    /** Parse helpers shared by the classic XCP and the XCPplus IF_DATA parser.
     *  Works on the token reader only; the ASAP2 grammar is not needed here. */
    class XcpCommonParser
    {
    public:
        /** Version plus the optional CAN parameters, stopping before DAQ_LIST_CAN_ID.
         *  @throws FormatException. */
        static void parseCanParametersWithoutDaqListCanId(A2lTokenReader & reader, XcpCanParameters & parameters);
        /** The caller has consumed "/begin <keyword>".
         *  @return false if @p keyword is not a common parameter block.
         *  @throws FormatException. */
        static bool parseCommonParameterContent(A2lTokenReader & reader, const std::string & keyword,
            std::unordered_set<std::string> & oneTimeKeywords, XcpCommonParameters & parameters);
        /** @throws FormatException. */
        static void parseDaqContent(A2lTokenReader & reader, XcpDaq & daq);
        /** The caller has consumed "/begin <keyword>".
         *  @return false if @p keyword is not DAQ_LIST_CAN_ID.
         *  @throws FormatException. */
        static bool parseDaqListCanIdContent(A2lTokenReader & reader, const std::string & keyword,
            XcpCanParameters & parameters);
        /** @throws FormatException. */
        static void parsePagContent(A2lTokenReader & reader, XcpPag & pag);
        /** @throws FormatException. */
        static void parseProtocolLayerContent(A2lTokenReader & reader, XcpProtocolLayer & layer);

    private:
        XcpCommonParser() = default;
        /** @throws FormatException. */
        static void parseCommunicationModeSupported(A2lTokenReader & reader, XcpProtocolLayer & layer);
        /** @throws FormatException. */
        static void parseEventContent(A2lTokenReader & reader, XcpDaq & daq);
    };

}
