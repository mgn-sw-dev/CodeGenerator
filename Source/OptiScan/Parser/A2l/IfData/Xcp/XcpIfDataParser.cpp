
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpCommonParser.h>
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpIfDataParser.h>
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpKeyword.h>
#include <memory>
#include <unordered_set>
#include <utility>

using namespace OptiScan::Parser::A2l::Mcd;
using namespace std;

namespace OptiScan::Parser::A2l::IfData::Xcp
{
	bool XcpPlusIfDataParser::canParse(const McdIdent & name) const
	{
		return name == XcpPlusIfData::Name;
	}

	McdIfDataPtr XcpPlusIfDataParser::parseContent(A2lTokenReader & reader)
	{
		unique_ptr<XcpPlusIfData> ifData = make_unique<XcpPlusIfData>();
		reader.parseUInt16(ifData->_marker);
		unordered_set<string> oneTimeKeywords;
		while (!reader.tryMatchToken(A2lTokenKind::EscapeEnd))
		{
			string keyword;
			reader.parseBlockBeginAny(keyword);
			if (XcpCommonParser::parseCommonParameterContent(reader, keyword, oneTimeKeywords, ifData->_parameters))
			{
				// handled
			}
			else if (keyword == XcpKeyword::XcpOnCan)
			{
				XcpPlusXcpOnCan tmp;
				XcpPlusIfDataParser::parseXcpOnCanContent(reader, tmp);
				ifData->_xcpOnCans.push_back(std::move(tmp));
			}
			else
			{
				reader.parseUnknownBlockContent();
			}
			reader.parseBlockEnd(keyword);
		}
		return ifData;
	}

	void XcpPlusIfDataParser::parseXcpOnCanContent(A2lTokenReader & reader, XcpPlusXcpOnCan & xcpOnCan)
	{
		XcpCommonParser::parseCanParametersWithoutDaqListCanId(reader, xcpOnCan);
		unordered_set<string> oneTimeKeywords;
		while (reader.tryMatchToken(A2lTokenKind::EscapeBegin))
		{
			string keyword;
			reader.parseBlockBeginAny(keyword);
			if (XcpCommonParser::parseDaqListCanIdContent(reader, keyword, xcpOnCan))
			{
				// handled
			}
			else if (XcpCommonParser::parseCommonParameterContent(reader, keyword, oneTimeKeywords,
				xcpOnCan._parameters))
			{
				// handled
			}
			else
			{
				reader.parseUnknownBlockContent();
			}
			reader.parseBlockEnd(keyword);
		}
		if (reader.tryMatchKeyword(XcpKeyword::TransportLayerInstance))
		{
			reader.readNextToken();
			string tmp;
			reader.parseString(tmp);
			xcpOnCan._transportLayerInstance = std::move(tmp);
		}
	}

}
