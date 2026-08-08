#include <OptiScan/Parser/Ldf/LdfKeyword.h>
#include <OptiScan/Parser/Ldf/LdfParser.h>
#include <OptiScan/Parser/FileException.h>
#include <OptiScan/Parser/TokenReaderUtils.h>
#include <sstream>

using namespace std;

namespace OptiScan::Parser::Ldf
{
	LdfParser::LdfParser(istream * input)
		: _ldfModes()
		, _reader(input)
	{
	}

	void LdfParser::getLdfModes(LdfModes & ldfModes) const
	{
		ldfModes = this->_ldfModes;
	}

	void LdfParser::parse(LdfDatabase & ldfDatabase)
	{
		ldfDatabase = LdfDatabase();
		this->_reader.readNextToken();
		this->parseDescriptionFile();
		this->parseProtocolVersion(ldfDatabase._protocolVersion);
		this->parseLanguageVersion(ldfDatabase._languageVersion);
		this->parseSpeed(ldfDatabase._speed_kBit_per_s);
		if (this->_reader.tryMatchKeyword(LdfKeyword::ChannelName))
		{
			this->parseChannelName(ldfDatabase._languageVersion, ldfDatabase._channelName);
		}
		this->parseNodes(ldfDatabase._masterNode, ldfDatabase._slaveNodes);
		if (this->_reader.tryMatchKeyword(LdfKeyword::Composite))
		{
			this->parseNodeComposite();
		}
		this->parseSignals(ldfDatabase._signals);
		if (this->_reader.tryMatchKeyword(LdfKeyword::DiagnosticSignals))
		{
			this->parseDiagnosticSignals();
		}
		this->parseFrames(ldfDatabase._frames);
		if (this->_reader.tryMatchKeyword(LdfKeyword::SporadicFrames))
		{
			this->parseSporadicFrames();
		}
		if (this->_reader.tryMatchKeyword(LdfKeyword::EventTriggeredFrames))
		{
			this->parseEventTriggeredFrames();
		}
		if (this->_reader.tryMatchKeyword(LdfKeyword::DiagnosticFrames))
		{
			this->parseDiagnosticFrames();
		}
		this->parseNodeAttributes();
		this->parseScheduleTables(ldfDatabase._languageVersion);
		// (deprecated, was 1.3) opt Signal_groups
		if (this->_reader.tryMatchKeyword(LdfKeyword::SignalEncodingTypes))
		{
			this->parseSignalEncodingTypes(ldfDatabase._signalEncodingTypes);
		}
		if (this->_reader.tryMatchKeyword(LdfKeyword::SignalRepresentation))
		{
			this->parseSignalRepresentations(ldfDatabase._signalRepresentations);
		}
		this->parseEndOfFile();
	}

	void LdfParser::parseChannelName(const std::string & languageVersion, std::string & channelName)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::ChannelName);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
		if (languageVersion == "2.1")
		{
			this->_reader.parseString(channelName);
		}
		else
		{
			this->_reader.parseIdentifier(channelName);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
	}

	void LdfParser::parseDescriptionFile()
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::LinDescriptionFile);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
	}
	
	void LdfParser::parseDiagnosticFrames()
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::DiagnosticFrames);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);		
		for (uint32_t frameIndex = 0; frameIndex < 2; frameIndex++)
		{
			string name;
			uint32_t expectedId;
			if (frameIndex == 0)
			{
				name = LdfKeyword::MasterReq;
				expectedId = 60;
			}
			else
			{
				name = LdfKeyword::SlaveResp;
				expectedId = 61;
			}
			this->_reader.matchKeywordAndRead(name);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorColon);
			uint32_t id;
			this->_reader.parseUInt32(id);
			if (id != expectedId)
			{
				throw FormatException(to_string(expectedId) + " expected");
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
			for (uint32_t i = 0; i < 8; i++)
			{
				string const tmp = name + "B" + to_string(i);
				this->_reader.matchKeywordAndRead(tmp);
				this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
				uint32_t offset;
				this->_reader.parseUInt32(offset);
				uint32_t const expectedOffset = 8 * i;
				if (offset != expectedOffset)
				{
					throw FormatException(to_string(expectedOffset) + " expected");
				}
				this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseDiagnosticSignals()
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::DiagnosticSignals);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		for (int i = 0; i < 16; i++)
		{
			string id;
			if (i < 8)
			{
				id = LdfKeyword::MasterReq + "B" + to_string(i);
			}
			else
			{
				id = LdfKeyword::SlaveResp + "B" + to_string(i - 8);
			}
			this->_reader.matchKeywordAndRead(id);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorColon);
			uint32_t size;
			this->_reader.parseUInt32(size);
			if (size != 8)
			{
				throw FormatException("8 expected");
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			uint32_t init;
			this->_reader.parseUInt32(init);
			if (init != 0)
			{
				throw FormatException("0 expected");
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}
	
	void LdfParser::parseEndOfFile() const
	{
		this->_reader.matchToken(LdfTokenKind::None);
	}

	void LdfParser::parseEventTriggeredFrames()
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::EventTriggeredFrames);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		bool nextFrame = true;
		while (nextFrame)
		{
			string frameName;
			this->_reader.parseIdentifier(frameName);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorColon);
			string collisionResolvingScheduleTable;
			this->_reader.parseIdentifier(collisionResolvingScheduleTable);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			uint32_t frameId;
			this->_reader.parseUInt32(frameId);
			bool nextItem = true;
			while (nextItem)
			{
				this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
				string itemName;
				this->_reader.parseIdentifier(itemName);
				nextItem = !this->_reader.tryMatchToken(LdfTokenKind::OperatorSemicolon);
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
			nextFrame = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}
	
	void LdfParser::parseFrames(std::vector<LinFrame> & frames)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::Frames);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		bool nextFrame = true;
		while (nextFrame)
		{
			LinFrame frame;
			this->_reader.parseIdentifier(frame._name);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorColon);
			this->_reader.parseUInt8(frame._id);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			this->_reader.parseIdentifier(frame._publishedBy);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			this->_reader.parseUInt8(frame._byteSize);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
			this->parseFrameSignals(frame._signals);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
			frames.push_back(frame);
			nextFrame = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseFrameSignals(std::vector<LinFrameSignal> & signals)
	{
		bool nextSignal = true;
		while (nextSignal)
		{
			LinFrameSignal signal;
			this->_reader.parseIdentifier(signal._name);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			this->_reader.parseUInt8(signal._offset);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
			signals.push_back(signal);
			nextSignal = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
	}

	void LdfParser::parseLanguageVersion(string & languageVersion)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::LinLanguageVersion);
		this->parseVersions(languageVersion);
	}

	void LdfParser::parseMasterNode(MasterNode & masterNode)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::Master);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorColon);
		this->_reader.parseIdentifier(masterNode._name);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		this->_reader.parseFloat64_ms(masterNode._timeBase_ms);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		this->_reader.parseFloat64_ms(masterNode._jitter_ms);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
	}

	void LdfParser::parseNodeAttributeConfigurableFrames(const pair<uint8_t, uint8_t> & protocolVersion)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::ConfigurableFrames);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		bool nextFrame = true;
		while (nextFrame)
		{
			string name;
			this->_reader.parseIdentifier(name);
			if (protocolVersion == pair<uint8_t, uint8_t>(2,0))
			{
				this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
				uint32_t messageId;
				this->_reader.parseUInt32(messageId);
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
			nextFrame = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
	}

	void LdfParser::parseNodeAttributeFaultStateSignals()
	{
		if (this->_reader.tryMatchKeyword(LdfKeyword::FaultStateSignals))
		{
			this->_reader.readNextToken();
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
			vector<string> faultSignals;
			bool nextSignal = true;
			while (nextSignal)
			{
				string tmp;
				this->_reader.parseIdentifier(tmp);
				faultSignals.push_back(tmp);
				nextSignal = this->_reader.tryMatchToken(LdfTokenKind::OperatorComma);
				if (nextSignal)
				{
					this->_reader.readNextToken();
				}
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		}
	}

	void LdfParser::parseNodeAttributeInitialNad()
	{
		if (this->_reader.tryMatchKeyword(LdfKeyword::InitialNad))
		{
			this->_reader.readNextToken();
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
			uint32_t initialNad;
			this->_reader.parseUInt32(initialNad);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		}
	}

	void LdfParser::parseNodeAttributeNasTimeout()
	{
		if (this->_reader.tryMatchKeyword(LdfKeyword::NAsTimeout))
		{
			this->_reader.readNextToken();
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
			double nAsTimeout_ms;
			this->_reader.parseFloat64_ms(nAsTimeout_ms);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		}
	}

	void LdfParser::parseNodeAttributeNcrTimeout()
	{
		if (this->_reader.tryMatchKeyword(LdfKeyword::NCrTimeout))
		{
			this->_reader.readNextToken();
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
			double nCrTimeout_ms;
			this->_reader.parseFloat64_ms(nCrTimeout_ms);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		}
	}

	void LdfParser::parseNodeAttributeP2Min()
	{
		if (this->_reader.tryMatchKeyword(LdfKeyword::P2Min))
		{
			this->_reader.readNextToken();
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
			double p2Min_ms;
			this->_reader.parseFloat64_ms(p2Min_ms);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		}
	}

	void LdfParser::parseNodeAttributeStMin()
	{
		if (this->_reader.tryMatchKeyword(LdfKeyword::StMin))
		{
			this->_reader.readNextToken();
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
			double stMin_ms;
			this->_reader.parseFloat64_ms(stMin_ms);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		}
	}
	
	void LdfParser::parseNodeAttributes()
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::NodeAttributes);		
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		bool nextNode = true;
		while (nextNode)
		{
			string name;
			this->_reader.parseIdentifier(name);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);

			string version;
			this->_reader.matchKeywordAndRead(LdfKeyword::LinProtocol);
			this->parseVersions(version);
			pair<uint8_t, uint8_t> protocolVersion = TokenReaderUtils::literalStringTokenTextToVersionPair(version);
			
			this->_reader.matchKeywordAndRead(LdfKeyword::ConfiguredNad);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
			uint32_t configuredNad;
			this->_reader.parseUInt32(configuredNad);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);

			this->parseNodeAttributeInitialNad();

			if (2 <= protocolVersion.first)
			{
				this->parseNodeAttributesForProtocolMajorVersion2(protocolVersion);
			}			
			
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
			
			nextNode = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseNodeAttributesForProtocolMajorVersion2(const pair<uint8_t, uint8_t> & protocolVersion)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::ProductId);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
		uint32_t supplierId;
		this->_reader.parseUInt32(supplierId);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint32_t functionId;
		this->_reader.parseUInt32(functionId);
		if (this->_reader.tryMatchToken(LdfTokenKind::OperatorComma))
		{
			this->_reader.readNextToken();
			uint32_t variant;
			this->_reader.parseUInt32(variant);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);

		this->_reader.matchKeywordAndRead(LdfKeyword::ResponseError);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
		string errorSignalName;
		this->_reader.parseIdentifier(errorSignalName);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);

		this->parseNodeAttributeFaultStateSignals();
		this->parseNodeAttributeP2Min();
		this->parseNodeAttributeStMin();
		this->parseNodeAttributeNasTimeout();
		this->parseNodeAttributeNcrTimeout();
		this->parseNodeAttributeConfigurableFrames(protocolVersion);

		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseNodes(MasterNode & masterNode, std::vector<std::string> & slaveNodes)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::Nodes);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		this->parseMasterNode(masterNode);

		this->_reader.matchKeywordAndRead(LdfKeyword::Slaves);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorColon);
		this->parseSlaveNodes(slaveNodes);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseNodeComposite()
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::Composite);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		bool nextConfig = true;
		while (nextConfig)
		{
			this->_reader.matchKeywordAndRead(LdfKeyword::Configuration);
			string configName;
			this->_reader.parseIdentifier(configName);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
			bool nextCompositeNode = true;
			while (nextCompositeNode)
			{
				string compositeNode;
				this->_reader.parseIdentifier(compositeNode);
				this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
				bool nextLogicalNode = true;
				while (nextLogicalNode)
				{
					string logicalNode;
					this->_reader.parseIdentifier(logicalNode);
					nextLogicalNode = this->_reader.tryMatchToken(LdfTokenKind::OperatorComma);
					if (nextLogicalNode)
					{
						this->_reader.readNextToken();
					}
				}
				this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
				this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
				nextCompositeNode = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
			nextConfig = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseProtocolVersion(string & protocolVersion)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::LinProtocolVersion);
		this->parseVersions(protocolVersion);
	}

	void LdfParser::parseScheduleTableAssignFrameId()
	{
		this->_reader.readNextToken();
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		string nodeName;
		this->_reader.parseIdentifier(nodeName);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		string frameName;
		this->_reader.parseIdentifier(frameName);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseScheduleTableAssignFrameIdRange()
	{
		this->_reader.readNextToken();
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		string nodeName;
		this->_reader.parseIdentifier(nodeName);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint32_t frameIndex;
		this->_reader.parseUInt32(frameIndex);
		if (this->_reader.tryMatchToken(LdfTokenKind::OperatorComma))
		{
			this->_reader.readNextToken();
			string framePid1;
			this->_reader.parseIdentifier(framePid1);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			string framePid2;
			this->_reader.parseIdentifier(framePid2);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			string framePid3;
			this->_reader.parseIdentifier(framePid3);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			string framePid4;
			this->_reader.parseIdentifier(framePid4);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseScheduleTableAssignNad()
	{
		this->_reader.readNextToken();
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		string nodeName;
		this->_reader.parseIdentifier(nodeName);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseScheduleTableConditionalChangeNad()
	{
		this->_reader.readNextToken();
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		uint8_t nad;
		this->_reader.parseUInt8(nad);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t id;
		this->_reader.parseUInt8(id);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t byte;
		this->_reader.parseUInt8(byte);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t mask;
		this->_reader.parseUInt8(mask);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t inv;
		this->_reader.parseUInt8(inv);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t newNad;
		this->_reader.parseUInt8(newNad);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseScheduleTableDataDump()
	{
		this->_reader.readNextToken();
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		string nodeName;
		this->_reader.parseIdentifier(nodeName);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d1;
		this->_reader.parseUInt8(d1);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d2;
		this->_reader.parseUInt8(d2);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d3;
		this->_reader.parseUInt8(d3);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d4;
		this->_reader.parseUInt8(d4);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d5;
		this->_reader.parseUInt8(d5);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseScheduleTableFreeFormat()
	{
		this->_reader.readNextToken();
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		uint8_t d1;
		this->_reader.parseUInt8(d1);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d2;
		this->_reader.parseUInt8(d2);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d3;
		this->_reader.parseUInt8(d3);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d4;
		this->_reader.parseUInt8(d4);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d5;
		this->_reader.parseUInt8(d5);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d6;
		this->_reader.parseUInt8(d6);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d7;
		this->_reader.parseUInt8(d7);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		uint8_t d8;
		this->_reader.parseUInt8(d8);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseScheduleTableSaveConfiguration()
	{
		this->_reader.readNextToken();
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		string nodeName;
		this->_reader.parseIdentifier(nodeName);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseScheduleTables(const std::string & languageVersion)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::ScheduleTables);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		
		bool nextTable = true;
		while (nextTable)
		{
			string name;
			this->_reader.parseIdentifier(name);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
			bool nextCommand = true;
			while (nextCommand)
			{
				// Command
				if (this->_reader.tryMatchKeyword(LdfKeyword::MasterReq))
				{
					this->_reader.readNextToken();
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::SlaveResp))
				{
					this->_reader.readNextToken();
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::AssignNad))
				{
					this->parseScheduleTableAssignNad();
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::ConditionalChangeNad))
				{
					this->parseScheduleTableConditionalChangeNad();
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::DataDump))
				{
					this->parseScheduleTableDataDump();
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::SaveConfiguration))
				{
					this->parseScheduleTableSaveConfiguration();
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::AssignFrameIdRange))
				{
					this->parseScheduleTableAssignFrameIdRange();
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::FreeFormat))
				{
					this->parseScheduleTableFreeFormat();
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::AssignFrameId) && languageVersion == "2.0")
				{
					this->parseScheduleTableAssignFrameId();
				}
				else
				{
					string frameName;
					this->_reader.parseIdentifier(frameName);
				}
				
				// Timing
				this->_reader.matchKeywordAndRead(LdfKeyword::Delay);
				double frameTime_ms;
				this->_reader.parseFloat64_ms(frameTime_ms);
				this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
				
				nextCommand = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
			
			nextTable = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
		
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseSignalEncodingTypeAsciiValue(LinSignalEncodingType & type)
	{
		this->_reader.readNextToken();
		LinAsciiValue linAsciiValue;
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		type._values.push_back(linAsciiValue);
	}

	void LdfParser::parseSignalEncodingTypeBcdValue(LinSignalEncodingType & type)
	{
		this->_reader.readNextToken();
		LinBcdValue linBcdValue;
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		type._values.push_back(linBcdValue);
	}

	void LdfParser::parseSignalEncodingTypeLogicalValue(LinSignalEncodingType & type)
	{
		this->_reader.readNextToken();
		LinLogicalValue logicalValue;
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		if (this->_ldfModes._allowSignalEncodingType32)
		{
			this->_reader.parseUInt32(logicalValue._signalValue);
		}
		else
		{
			try
			{
				uint16_t value;
				this->_reader.parseUInt16(value);
				logicalValue._signalValue = value;
			}
			catch (const FormatException & e)
			{
				throw FormatException("LdfParser: SignalEncodingType for signal value must be uint32.");
			}
		}
		if (this->_reader.tryMatchToken(LdfTokenKind::OperatorComma))
		{
			this->_reader.readNextToken();
			this->_reader.parseString(logicalValue._textInfo);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		type._values.push_back(logicalValue);
	}

	void LdfParser::parseSignalEncodingTypePhysicalValue(LinSignalEncodingType & type)
	{
		this->_reader.readNextToken();
		LinPhysicalValue physicalValue;
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		if (this->_ldfModes._allowSignalEncodingType32)
		{
			this->_reader.parseUInt32(physicalValue._minValue);
		}
		else
		{
			try
			{
				uint16_t value;
				this->_reader.parseUInt16(value);
				physicalValue._minValue = value;
			}
			catch (const FormatException & e)
			{
				throw FormatException("LdfParser: SignalEncodingType for min value must be uint32.");
			}

		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		if (this->_ldfModes._allowSignalEncodingType32)
		{
			this->_reader.parseUInt32(physicalValue._maxValue);
		}
		else
		{
			try
			{
				uint16_t value;
				this->_reader.parseUInt16(value);
				physicalValue._maxValue = value;
			}
			catch (const FormatException & e)
			{
				throw FormatException("LdfParser: SignalEncodingType for max value must be uint32.");
			}

		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		this->_reader.parseFloat64(physicalValue._scale);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
		this->_reader.parseFloat64(physicalValue._offset);
		if (this->_reader.tryMatchToken(LdfTokenKind::OperatorComma))
		{
			this->_reader.readNextToken();
			this->_reader.parseString(physicalValue._textInfo);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
		type._values.push_back(physicalValue);
	}

	void LdfParser::parseSignalEncodingTypes(vector<LinSignalEncodingType> & signalEncodingTypes)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::SignalEncodingTypes);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);		
		bool nextType = true;
		while (nextType)
		{
			LinSignalEncodingType type;
			this->_reader.parseIdentifier(type._name);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
			
			bool nextValue = true;
			while (nextValue)
			{
				if (this->_reader.tryMatchKeyword(LdfKeyword::LogicalValue))
				{
					this->parseSignalEncodingTypeLogicalValue(type);
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::PhysicalValue))
				{
					this->parseSignalEncodingTypePhysicalValue(type);
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::BcdValue))
				{
					this->parseSignalEncodingTypeBcdValue(type);
				}
				else if (this->_reader.tryMatchKeyword(LdfKeyword::AsciiValue))
				{
					this->parseSignalEncodingTypeAsciiValue(type);
				}
				else
				{
					throw FormatException("Unknown value");
				}
				nextValue = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);			
			signalEncodingTypes.push_back(type);
			
			nextType = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}		
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}
	
	void LdfParser::parseSignalRepresentations(vector<LinSignalRepresentation> & signalRepresentations)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::SignalRepresentation);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		bool nextType = true;
		while (nextType)
		{
			LinSignalRepresentation representation;
			this->_reader.parseIdentifier(representation._encodingType);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorColon);			
			bool nextSignal= true;
			while (nextSignal)
			{
				string tmp;
				this->_reader.parseIdentifier(tmp);
				representation._signals.push_back(tmp);
				nextSignal = this->_reader.tryMatchToken(LdfTokenKind::OperatorComma);
				if (nextSignal)
				{
					this->_reader.readNextToken();
				}
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
			signalRepresentations.push_back(representation);
			nextType = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseSignals( std::vector<LinSignal> & signals)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::Signals);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		bool checkNext = true;
		while (checkNext)
		{
			LinSignal signal;
			this->_reader.parseIdentifier(signal._name);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorColon);
			this->_reader.parseUInt8(signal._bitSize);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			this->parseSignalInitValues(signal);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			this->_reader.parseIdentifier(signal._publishedBy);
			if (!this->_ldfModes._allowSignalWithoutSubscriber)
			{
				// must be at least one subscriber
				try
				{
					this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
				}
				catch (const FormatException & error)
				{
					throw FormatException("LdfParser: Signal without subscriber");
				}
				string subscriber;
				this->_reader.parseIdentifier(subscriber);
				signal._subscribedBy.push_back(subscriber);
			}
			while (this->_reader.tryMatchToken(LdfTokenKind::OperatorComma))
			{
				this->_reader.readNextToken();
				string subscriber;
				this->_reader.parseIdentifier(subscriber);
				signal._subscribedBy.push_back(subscriber);
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
			signals.push_back(signal);
			checkNext = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}

	void LdfParser::parseSignalInitValues(LinSignal & signal)
	{
		if (this->_reader.tryMatchToken(LdfTokenKind::OperatorLeftCurlyBracket))
		{
			// Array with at least one element
			switch (signal._bitSize)
			{
			default:
				throw FormatException("Byte array signal size out of range");
			case 8:
			case 16:
			case 24:
			case 32:
			case 40:
			case 48:
			case 56:
			case 64:
				break;
			}
			this->_reader.readNextToken();
			LinByteArraySignal value;
			uint8_t byte;
			this->_reader.parseUInt8(byte);
			value._initValueBigEndian.push_back(byte);
			while (this->_reader.tryMatchToken(LdfTokenKind::OperatorComma))
			{
				this->_reader.readNextToken();
				this->_reader.parseUInt8(byte);
				value._initValueBigEndian.push_back(byte);
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
			signal._value = value;
			// signal._valueType = Array
		}
		else
		{
			// Scalar
			if (signal._bitSize < 1 || 16 < signal._bitSize )
			{
				throw FormatException("Scalar signal size out of range");
			}
			LinScalarSignal value;
			this->_reader.parseUInt16(value._initValue);
			signal._value = value;
			// signal._valueType = Scalar
		}
	}

	void LdfParser::parseSporadicFrames()
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::SporadicFrames);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorLeftCurlyBracket);
		bool nextFrame = true;
		while (nextFrame)
		{
			string frameName;
			this->_reader.parseIdentifier(frameName);
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorColon);
			bool nextItem = true;
			while (nextItem)
			{
				string itemName;
				this->_reader.parseIdentifier(itemName);
				nextItem = this->_reader.tryMatchToken(LdfTokenKind::OperatorComma);
				if (nextItem)
				{
					this->_reader.readNextToken();
				}
			}
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
			nextFrame = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
		}
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorRightCurlyBracket);
	}
	
	void LdfParser::parseSlaveNodes(std::vector<std::string> & slaveNodes)
	{
		std::string name;
		this->_reader.parseIdentifier(name);
		slaveNodes.push_back(name);
		while (this->_reader.tryMatchToken(LdfTokenKind::OperatorComma))
		{
			this->_reader.readNextToken();
			this->_reader.parseIdentifier(name);
			slaveNodes.push_back(name);
		}
	}

	void LdfParser::parseSpeed(double & speed_kBit_per_s)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::LinSpeed);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
		this->_reader.parseFloat64(speed_kBit_per_s);
		this->_reader.matchKeywordAndRead(LdfKeyword::Kbps);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
	}

	void LdfParser::parseVersions(std::string & version)
	{
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
		this->_reader.matchToken(LdfTokenKind::LiteralString);
		version = TokenReaderUtils::literalStringTokenTextToString(this->_reader.token()._text);
		this->_reader.readNextToken();
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
	}

	void LdfParser::setLdfModes(const LdfModes & ldfModes)
	{
		this->_ldfModes = ldfModes;
	}

}
