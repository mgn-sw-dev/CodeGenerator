
#include <OptiScan/Parser/Ldf/LdfParser.h>

#include "LdfKeyword.h"
#include "OptiScan/Parser/FileException.h"
#include "OptiScan/Parser/TokenReaderUtils.h"

using namespace std;

namespace OptiScan::Parser::Ldf
{
	LdfParser::LdfParser(istream * input)
		: _modes(Mode::None)
		, _reader(input)
	{
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

	void LdfParser::parseSignals( std::vector<LinSignal> & _signals)
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
			this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
			this->_reader.parseIdentifier(signal._publishedBy);
			if (!this->_modes.testFlag(LdfParser::Mode::AllowSignalWithoutSubscriber))
			{
				this->_reader.matchTokenAndRead(LdfTokenKind::OperatorComma);
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
			_signals.push_back(signal);
			checkNext = !this->_reader.tryMatchToken(LdfTokenKind::OperatorRightCurlyBracket);
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
}
