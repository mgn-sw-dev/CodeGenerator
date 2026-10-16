
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpCommonParser.h>
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpKeyword.h>
#include <OptiScan/Parser/FileException.h>
#include <utility>

using namespace std;

namespace OptiScan::Parser::A2l::IfData::Xcp
{
	void XcpCommonParser::parseCanParametersWithoutDaqListCanId(A2lTokenReader & reader, XcpCanParameters & parameters)
	{
		reader.parseUInt16(parameters._version);
		unordered_set<string> oneTimeKeywords;
		bool checkNext = true;
		while (checkNext)
		{
			checkNext = reader.tryMatchToken(A2lTokenKind::Identifier);
			if (checkNext)
			{
				string const keyword = reader.token()._text;
				if (keyword == XcpKeyword::CanIdBroadcast)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					XcpCanId tmp;
					reader.parseUInt32(tmp._value);
					parameters._canIdBroadcast = tmp;
				}
				else if (keyword == XcpKeyword::CanIdMaster)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					XcpCanId tmp;
					reader.parseUInt32(tmp._value);
					parameters._canIdMaster = tmp;
				}
				else if (keyword == XcpKeyword::CanIdSlave)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					XcpCanId tmp;
					reader.parseUInt32(tmp._value);
					parameters._canIdSlave = tmp;
				}
				else if (keyword == XcpKeyword::Baudrate)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					uint32_t tmp = 0;
					reader.parseUInt32(tmp);
					parameters._baudrate_Hz = tmp;
				}
				else if (keyword == XcpKeyword::SamplePoint)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					uint8_t tmp = 0;
					reader.parseUInt8(tmp);
					parameters._samplePoint_percentage = tmp;
				}
				else if (keyword == XcpKeyword::SampleRate)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					reader.parseEnumValue(XcpEnumText::CanSampleRate, "xcp can sample rate", parameters._sampleRate);
				}
				else if (keyword == XcpKeyword::BtlCycles)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					uint8_t tmp = 0;
					reader.parseUInt8(tmp);
					parameters._btlCycles = tmp;
				}
				else if (keyword == XcpKeyword::Sjw)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					uint8_t tmp = 0;
					reader.parseUInt8(tmp);
					parameters._sjw = tmp;
				}
				else if (keyword == XcpKeyword::SyncEdge)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					reader.parseEnumValue(XcpEnumText::CanSyncEdge, "xcp can sync edge", parameters._syncEdge);
				}
				else if (keyword == XcpKeyword::MaxDlcRequired)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					reader.readNextToken();
					parameters._isMaxDlcRequired = true;
				}
				else
				{
					checkNext = false;
				}
			}
		}
	}

	bool XcpCommonParser::parseCommonParameterContent(A2lTokenReader & reader, const string & keyword,
		unordered_set<string> & oneTimeKeywords, XcpCommonParameters & parameters)
	{
		bool result = true;
		if (keyword == XcpKeyword::Daq)
		{
			A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
			XcpDaq tmp;
			XcpCommonParser::parseDaqContent(reader, tmp);
			parameters._daq = std::move(tmp);
		}
		else if (keyword == XcpKeyword::Pag)
		{
			A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
			XcpPag tmp;
			XcpCommonParser::parsePagContent(reader, tmp);
			parameters._pag = tmp;
		}
		else if (keyword == XcpKeyword::ProtocolLayer)
		{
			A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
			XcpProtocolLayer tmp;
			XcpCommonParser::parseProtocolLayerContent(reader, tmp);
			parameters._protocolLayer = std::move(tmp);
		}
		else if (keyword == XcpKeyword::DaqEvent
			|| keyword == XcpKeyword::Pgm
			|| keyword == XcpKeyword::Segment)
		{
			// Recognised but not modelled, as in the old parser.
			A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
			reader.parseUnknownBlockContent();
		}
		else
		{
			result = false;
		}
		return result;
	}

	void XcpCommonParser::parseCommunicationModeSupported(A2lTokenReader & reader, XcpProtocolLayer & layer)
	{
		if (reader.tryMatchKeyword(XcpKeyword::Block))
		{
			reader.readNextToken();
			XcpCommunicationModeBlock tmp;
			bool hasMaster = false;
			bool hasSlave = false;
			bool checkNext = true;
			while (checkNext)
			{
				if (reader.tryMatchKeyword(XcpKeyword::Slave))
				{
					if (hasSlave)
					{
						reader.throwFormatError("XcpCommonParser::parseCommunicationModeSupported",
							"Keyword 'SLAVE' already encountered");
					}
					reader.readNextToken();
					tmp._slaveIsSupported = true;
					hasSlave = true;
				}
				else if (reader.tryMatchKeyword(XcpKeyword::Master))
				{
					if (hasMaster)
					{
						reader.throwFormatError("XcpCommonParser::parseCommunicationModeSupported",
							"Keyword 'MASTER' already encountered");
					}
					reader.readNextToken();
					tmp._masterIsSupported = true;
					reader.parseUInt8(tmp._masterMaxBs);
					reader.parseUInt8(tmp._masterMaxSt);
					hasMaster = true;
				}
				else
				{
					checkNext = false;
				}
			}
			layer._communicationModeSupported = tmp;
		}
		else
		{
			reader.matchKeyword(XcpKeyword::Interleaved);
			reader.readNextToken();
			XcpCommunicationModeInterleaved tmp;
			reader.parseUInt8(tmp._queueSize);
			layer._communicationModeSupported = tmp;
		}
	}

	void XcpCommonParser::parseDaqContent(A2lTokenReader & reader, XcpDaq & daq)
	{
		if (reader.tryMatchKeyword(XcpKeyword::Static))
		{
			daq._isDynamic = false;
		}
		else
		{
			reader.matchKeyword(XcpKeyword::Dynamic);
			daq._isDynamic = true;
		}
		reader.readNextToken();
		reader.parseUInt16(daq._maxDaq);
		reader.parseUInt16(daq._maxEventChannel);
		reader.parseUInt8(daq._minDaq);
		reader.parseEnumValue(XcpEnumText::OptimizationType, "xcp optimization type", daq._optimizationType);
		reader.parseEnumValue(XcpEnumText::AddressExtension, "xcp address extension", daq._addressExtension);
		reader.parseEnumValue(XcpEnumText::IdentificationFieldType, "xcp identification field type",
			daq._identificationField);
		reader.parseEnumValue(XcpEnumText::GranularityOdtEntrySizeDaq, "xcp granularity odt entry size daq",
			daq._granularityOdtEntrySizeDaq);
		reader.parseUInt8(daq._maxOdtEntrySizeDaq);
		reader.parseEnumValue(XcpEnumText::OverloadIndication, "xcp overload indication", daq._overloadIndication);

		unordered_set<string> oneTimeKeywords;
		while (!reader.tryMatchToken(A2lTokenKind::EscapeEnd))
		{
			if (reader.tryMatchToken(A2lTokenKind::Identifier))
			{
				string const keyword = reader.token()._text;
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				if (keyword == XcpKeyword::PidOffSupported)
				{
					reader.readNextToken();
					daq._isPidOffSupported = true;
				}
				else if (keyword == XcpKeyword::PrescalerSupported)
				{
					reader.readNextToken();
					daq._isPrescalerSupported = true;
				}
				else if (keyword == XcpKeyword::ResumeSupported)
				{
					reader.readNextToken();
					daq._isResumeSupported = true;
				}
				else
				{
					reader.throwFormatError("XcpCommonParser::parseDaqContent", "Unknown keyword '" + keyword + "'");
				}
			}
			else
			{
				string keyword;
				reader.parseBlockBeginAny(keyword);
				if (keyword == XcpKeyword::Event)
				{
					XcpCommonParser::parseEventContent(reader, daq);
				}
				else
				{
					// DAQ_LIST, STIM, TIMESTAMP_SUPPORTED and anything else are not modelled.
					reader.parseUnknownBlockContent();
				}
				reader.parseBlockEnd(keyword);
			}
		}
	}

	bool XcpCommonParser::parseDaqListCanIdContent(A2lTokenReader & reader, const string & keyword,
		XcpCanParameters & parameters)
	{
		bool const result = keyword == XcpKeyword::DaqListCanId;
		if (result)
		{
			XcpDaqListCanId id;
			reader.parseUInt16(id._daqListNumber);
			unordered_set<string> oneTimeKeywords;
			bool checkNext = true;
			while (checkNext)
			{
				checkNext = reader.tryMatchToken(A2lTokenKind::Identifier);
				if (checkNext)
				{
					string const entry = reader.token()._text;
					if (entry == XcpKeyword::Variable)
					{
						A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, entry);
						reader.readNextToken();
						id._isVariable = true;
					}
					else if (entry == XcpKeyword::Fixed)
					{
						A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, entry);
						reader.readNextToken();
						XcpCanId tmp;
						reader.parseUInt32(tmp._value);
						id._fixedTo = tmp;
					}
					else
					{
						checkNext = false;
					}
				}
			}
			parameters._daqListCanIds.push_back(id);
		}
		return result;
	}

	void XcpCommonParser::parseEventContent(A2lTokenReader & reader, XcpDaq & daq)
	{
		XcpEvent tmp;
		reader.parseString(tmp._eventChannelName);
		reader.parseString(tmp._eventChannelShortName);
		reader.parseUInt16(tmp._eventChannelNumber);
		XcpEventKind kind = XcpEventKind::Daq;
		reader.parseEnumValue(XcpEnumText::EventKind, "xcp daq event kind", kind);
		tmp._canDaq = kind == XcpEventKind::Daq || kind == XcpEventKind::DaqStim;
		tmp._canStim = kind == XcpEventKind::Stim || kind == XcpEventKind::DaqStim;
		reader.parseUInt8(tmp._maxDaqList);
		reader.parseUInt8(tmp._timeCycle);
		uint8_t timeUnitId = 0;
		reader.parseUInt8(timeUnitId);
		tmp._timeUnit = XcpTimeUnit(timeUnitId);
		reader.parseUInt8(tmp._priority);
		daq._events.push_back(std::move(tmp));
	}

	void XcpCommonParser::parsePagContent(A2lTokenReader & reader, XcpPag & pag)
	{
		reader.parseUInt8(pag._maxSegments);
		if (reader.tryMatchKeyword(XcpKeyword::FreezeSupported))
		{
			reader.readNextToken();
			pag._isFreezeSupported = true;
		}
	}

	void XcpCommonParser::parseProtocolLayerContent(A2lTokenReader & reader, XcpProtocolLayer & layer)
	{
		reader.parseUInt16(layer._version);
		reader.parseUInt16(layer._t1_ms);
		reader.parseUInt16(layer._t2_ms);
		reader.parseUInt16(layer._t3_ms);
		reader.parseUInt16(layer._t4_ms);
		reader.parseUInt16(layer._t5_ms);
		reader.parseUInt16(layer._t6_ms);
		reader.parseUInt16(layer._t7_ms);
		reader.parseUInt8(layer._maxCto);
		reader.parseUInt16(layer._maxDto);
		reader.parseEnumValue(XcpEnumText::ByteOrder, "xcp byte order", layer._byteOrder);
		reader.parseEnumValue(XcpEnumText::AddressGranularity, "xcp address granularity", layer._addressGranularity);

		unordered_set<string> oneTimeKeywords;
		while (!reader.tryMatchToken(A2lTokenKind::EscapeEnd))
		{
			reader.matchToken(A2lTokenKind::Identifier);
			string const keyword = reader.token()._text;
			if (keyword == XcpKeyword::OptionalCmd)
			{
				// May occur more than once.
				reader.readNextToken();
				XcpOptionalCommand tmp = XcpOptionalCommand::GetId;
				reader.parseEnumValue(XcpEnumText::OptionalCommand, "xcp optional command", tmp);
				layer._optionalCommands.insert(tmp);
			}
			else if (keyword == XcpKeyword::CommunicationModeSupported)
			{
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				reader.readNextToken();
				XcpCommonParser::parseCommunicationModeSupported(reader, layer);
			}
			else if (keyword == XcpKeyword::SeedAndKeyExternalFunction)
			{
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				reader.readNextToken();
				string tmp;
				reader.parseString(tmp);
				layer._seedAndKeyExternalFunction = std::move(tmp);
			}
			else
			{
				reader.throwFormatError("XcpCommonParser::parseProtocolLayerContent",
					"Unknown keyword '" + keyword + "'");
			}
		}
	}

}
