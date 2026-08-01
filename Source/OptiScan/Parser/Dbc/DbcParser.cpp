
#include <OptiScan/Parser/Dbc/DbcParser.h>
#include <OptiScan/Parser/Dbc/DbcException.h>
#include <OptiScan/Parser/Dbc/DbcKeyword.h>

using namespace std;

namespace OptiScan::Parser::Dbc
{
	DbcParser::DbcParser(istream * input)
		: _reader(input)
		, _observer(nullptr)
	{
	}
	
	DbcParserObserver * DbcParser::observer() const
	{
		return this->_observer;
	}

	void DbcParser::parseAttribute(DbcDatabase & dbcDatabase)
	{
		DbcToken const token = this->_reader.token();
		this->_reader.matchKeyword(DbcKeyword::BA_);
		this->_reader.readNextToken();
		string name;
		this->_reader.parseString(name);
		bool hasValue = true;
		if (!this->_reader.tryMatchToken(DbcTokenKind::Identifier))
		{
			this->parseAttributeGlobal(name, dbcDatabase._attributes);
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::BO_))
		{
			this->_reader.readNextToken();
			this->parseAttributeMessage(name, dbcDatabase._attributeMessages);
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::BU_))
		{
			this->_reader.readNextToken();
			this->parseAttributeNode(name, dbcDatabase._attributeNodes);
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::EV_))
		{
			this->_reader.readNextToken();
			this->parseAttributeEnvVar(name, dbcDatabase._attributeEnvVars);
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::SG_))
		{
			this->_reader.readNextToken();
			this->parseAttributeSignal(name, dbcDatabase._attributeSignals);
		}
		else
		{
			hasValue = false;
		}
		if (hasValue)
		{
			this->_reader.matchToken(DbcTokenKind::OperatorSemicolon);
			this->_reader.readNextToken();
			this->parseEndOfLine();
		}
		else
		{
			this->parseTailOfUnknownKeywordLine(token);
		}
	}

	void DbcParser::parseAttributeDef(vector<DbcAttributeDef> & attributeDefs)
	{
		DbcToken const token = this->_reader.token();
		this->_reader.matchKeyword(DbcKeyword::BA_DEF_);
		this->_reader.readNextToken();
		bool hasObjectType = true;
		DbcAttributeObjectType objectType = DbcAttributeObjectType::Unknown;
		if (this->_reader.tryMatchToken(DbcTokenKind::LiteralString))
		{
			objectType = DbcAttributeObjectType::Network;
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::BO_))
		{
			this->_reader.readNextToken();
			objectType = DbcAttributeObjectType::Message;
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::BU_))
		{
			this->_reader.readNextToken();
			objectType = DbcAttributeObjectType::Node;
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::EV_))
		{
			this->_reader.readNextToken();
			objectType = DbcAttributeObjectType::EnvVar;
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::SG_))
		{
			this->_reader.readNextToken();
			objectType = DbcAttributeObjectType::Signal;
		}
		else
		{
			hasObjectType = false;
		}
		DbcAttributeDef item;
		bool hasDef = true;
		if (hasObjectType)
		{
			string name;
			this->_reader.parseString(name);
			item._name = name;
			if (this->_reader.tryMatchKeyword("ENUM"))
			{
				this->_reader.readNextToken();
				DbcAttributeDefEnum defEnum;
				bool checkNext = this->_reader.tryMatchToken(DbcTokenKind::LiteralString);
				while (checkNext)
				{
					string value;
					this->_reader.parseString(value);
					defEnum._values.push_back(value);
					checkNext = this->_reader.tryMatchToken(DbcTokenKind::OperatorComma);
					if (checkNext)
					{
						this->_reader.readNextToken();
					}
				}
				item._def = defEnum;
			}
			else if (this->_reader.tryMatchKeyword("FLOAT"))
			{
				this->_reader.readNextToken();
				DbcAttributeDefFloat defFloat;
				this->_reader.parseFloat64(defFloat._minimum);
				this->_reader.parseFloat64(defFloat._maximum);
				item._def = defFloat;
			}
			else if (this->_reader.tryMatchKeyword("HEX"))
			{
				this->_reader.readNextToken();
				DbcAttributeDefHex defHex;
				this->_reader.parseUInt32(defHex._minimum);
				this->_reader.parseUInt32(defHex._maximum);
				item._def = defHex;
			}
			else if (this->_reader.tryMatchKeyword("INT"))
			{
				this->_reader.readNextToken();
				DbcAttributeDefInt defInt;
				this->_reader.parseInt32(defInt._minimum);
				this->_reader.parseInt32(defInt._maximum);
				item._def = defInt;
			}
			else if (this->_reader.tryMatchKeyword("STRING"))
			{
				this->_reader.readNextToken();
				DbcAttributeDefString defString;
				item._def = defString;
			}
			else
			{
				hasDef = false;
			}
		}
		if (!hasDef)
		{
			this->parseTailOfUnknownKeywordLine(token);
		}
		else
		{
			item._objectType = objectType;
			this->_reader.matchToken(DbcTokenKind::OperatorSemicolon);
			this->_reader.readNextToken();
			this->parseEndOfLine();
			attributeDefs.push_back(item);
		}
	}

	void DbcParser::parseAttributeDefault(vector<DbcAttribute> & attributeDefaults)
	{
		DbcAttribute item;
		this->_reader.matchKeyword(DbcKeyword::BA_DEF_DEF_);
		this->_reader.readNextToken();
		this->_reader.parseString(item._name);
		this->parseAttributeValue(item._value);
		this->_reader.matchToken(DbcTokenKind::OperatorSemicolon);
		this->_reader.readNextToken();
		this->parseEndOfLine();
		attributeDefaults.push_back(item);
	}

	void DbcParser::parseAttributeEnvVar(const string & name, vector<DbcAttributeEnvVar> & attributeEnvVars)
	{
		DbcAttributeEnvVar item;
		item._name = name;
		this->_reader.parseIdentifier(item._envVarName);
		this->parseAttributeValue(item._value);
		attributeEnvVars.push_back(item);
	}

	void DbcParser::parseAttributeGlobal(const string & name, vector<DbcAttribute> & attributes)
	{
		DbcAttribute item;
		item._name = name;
		this->parseAttributeValue(item._value);
		attributes.push_back(item);
	}

	void DbcParser::parseAttributeMessage(const string & name, vector<DbcAttributeMessage> & attributeMessages)
	{
		DbcAttributeMessage item;
		item._name = name;
		this->_reader.parseUInt32(item._messageId);
		this->parseAttributeValue(item._value);
		attributeMessages.push_back(item);
	}

	void DbcParser::parseAttributeNode(const string & name, vector<DbcAttributeNode> & attributeNodes)
	{
		DbcAttributeNode item;
		item._name = name;
		this->_reader.parseIdentifier(item._nodeName);
		this->parseAttributeValue(item._value);
		attributeNodes.push_back(item);
	}

	void DbcParser::parseAttributeSignal(const string & name, vector<DbcAttributeSignal> & attributeSignals)
	{
		DbcAttributeSignal item;
		item._name = name;
		this->_reader.parseUInt32(item._messageId);
		this->_reader.parseIdentifier(item._signalName);
		this->parseAttributeValue(item._value);
		attributeSignals.push_back(item);
	}

	void DbcParser::parseAttributeValue(DbcAttributeValue & value)
	{
		if (this->_reader.tryMatchToken(DbcTokenKind::Identifier))
		{
			string tmp;
			this->_reader.parseIdentifier(tmp);
			value = tmp;
		}
		else if(this->_reader.tryMatchToken(DbcTokenKind::LiteralString))
		{
			string tmp;
			this->_reader.parseString(tmp);
			value = tmp;
		}
		else
		{
			exception_ptr error;
			{
				uint32_t tmp;
				error = this->_reader.tryParseUInt32(tmp);
				if (!error)
				{
					value = tmp;
				}
			}
			if (error)
			{
				int32_t tmp;
				error = this->_reader.tryParseInt32(tmp);
				if (!error)
				{
					value = tmp;
				}
			}
			if (error)
			{
				double tmp;
				this->_reader.parseFloat64(tmp);
				value = tmp;
			}
		}
	}

	void DbcParser::parse(DbcDatabase & dbcDatabase)
	{
		dbcDatabase = DbcDatabase();

		this->_reader.readNextToken();
		if (this->_reader.tryMatchToken(DbcTokenKind::EndOfLine))
		{
			this->parseEndOfLine();
		}

		if (this->_reader.tryMatchKeyword(DbcKeyword::VERSION))
		{
			this->parseVersion(dbcDatabase._version);
		}

		if (this->_reader.tryMatchKeyword(DbcKeyword::NS_))
		{
			this->parseNewSymbols(dbcDatabase._newSymbols);
		}

		this->parseBitTiming(dbcDatabase._bitTiming);

		while (!this->_reader.tryMatchToken(DbcTokenKind::None))
		{
			this->_reader.matchToken(DbcTokenKind::Identifier);
			const string & keyword = this->_reader.token()._text;
			auto it = KeywordMap.find(keyword);
			DbcKeywordKind kind = DbcKeywordKind::Unknown;
			if (it != KeywordMap.end())
			{
				kind = it->second;
			}
			switch (kind)
			{
			case DbcKeywordKind::BA_:
				this->parseAttribute(dbcDatabase);
				break;
			case DbcKeywordKind::BA_DEF_:
				this->parseAttributeDef(dbcDatabase._attributeDefs);
				break;
			case DbcKeywordKind::BA_DEF_DEF_:
				this->parseAttributeDefault(dbcDatabase._attributeDefaults);
				break;
			case DbcKeywordKind::BO_:
				this->parseMessage(dbcDatabase._messages);
				break;
			case DbcKeywordKind::BO_TX_BU_:
				this->parseMessageTransmitters(dbcDatabase._messageTransmitters);
				break;
			case DbcKeywordKind::BU_:
				this->parseNodes(dbcDatabase._nodes);
				break;
			case DbcKeywordKind::CM_:
				this->parseComment(dbcDatabase);
				break;
			case DbcKeywordKind::SG_MUL_VAL_:
				this->parseExtendedMultiplexing(dbcDatabase._extendedMultiplexing);
				break;
			case DbcKeywordKind::SGTYPE_:
				this->parseSignalTypeOrReference(dbcDatabase);
				break;
			case DbcKeywordKind::SIG_GROUP_:
				this->parseSignalGroup(dbcDatabase._signalGroups);
				break;
			case DbcKeywordKind::VAL_:
				this->parseValueDescriptions(dbcDatabase);
				break;
			case DbcKeywordKind::VAL_TABLE_:
				this->parseValueTable(dbcDatabase._valueTables);
				break;
			case DbcKeywordKind::BS_:
			case DbcKeywordKind::EV_:
			case DbcKeywordKind::SG_:
			case DbcKeywordKind::NS_:
			case DbcKeywordKind::VERSION:
			case DbcKeywordKind::Unknown:
			default:
				{
					DbcToken const token = this->_reader.token();
					this->parseTailOfUnknownKeywordLine(token);
				}
				break;
			}
		}
	}

	void DbcParser::parseBitTiming(DbcBitTiming & bitTiming)
	{
		this->_reader.matchKeyword(DbcKeyword::BS_);
		this->_reader.readNextToken();
		this->_reader.matchToken(DbcTokenKind::OperatorColon);
		this->_reader.readNextToken();
		if (!this->_reader.tryMatchToken(DbcTokenKind::EndOfLine))
		{
			this->_reader.parseUInt32(bitTiming._baudrate);
			this->_reader.matchToken(DbcTokenKind::OperatorColon);
			this->_reader.readNextToken();
			this->_reader.parseUInt32(bitTiming._btr1);
			this->_reader.matchToken(DbcTokenKind::OperatorComma);
			this->_reader.readNextToken();
			this->_reader.parseUInt32(bitTiming._btr2);
		}
		this->parseEndOfLine();
	}

	void DbcParser::parseComment(DbcDatabase & dbcDatabase)
	{
		DbcToken const token = this->_reader.token();
		this->_reader.matchKeyword(DbcKeyword::CM_);
		this->_reader.readNextToken();
		bool hasMatch = true;
		if (this->_reader.tryMatchToken(DbcTokenKind::LiteralString))
		{
			this->parseCommentGlobal(dbcDatabase._comments);
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::BO_))
		{
			this->_reader.readNextToken();
			this->parseCommentMessage(dbcDatabase._commentMessages);
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::BU_))
		{
			this->_reader.readNextToken();
			this->parseCommentNode(dbcDatabase._commentNodes);
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::EV_))
		{
			this->_reader.readNextToken();
			this->parseCommentEnvVar(dbcDatabase._commentEnvVars);
		}
		else if (this->_reader.tryMatchKeyword(DbcKeyword::SG_))
		{
			this->_reader.readNextToken();
			this->parseCommentSignal(dbcDatabase._commentSignals);
		}
		else
		{
			hasMatch = false;
		}
		if (hasMatch)
		{
			this->_reader.matchToken(DbcTokenKind::OperatorSemicolon);
			this->_reader.readNextToken();
			this->parseEndOfLine();
		}
		else
		{
			this->parseTailOfUnknownKeywordLine(token);
		}
	}

	void DbcParser::parseCommentEnvVar(vector<DbcCommentEnvVar> & commentEnvVars)
	{
		DbcCommentEnvVar commentEnvVar;
		this->_reader.parseIdentifier(commentEnvVar._envVarName);
		this->_reader.parseString(commentEnvVar._text);
		commentEnvVars.push_back(commentEnvVar);
	}

	void DbcParser::parseCommentGlobal(vector<DbcComment> & comments)
	{
		DbcComment comment;
		this->_reader.parseString(comment._text);
		comments.push_back(comment);
	}

	void DbcParser::parseCommentMessage(vector<DbcCommentMessage> & commentMessages)
	{
		DbcCommentMessage commentMessage;
		this->_reader.parseUInt32(commentMessage._messageId);
		this->_reader.parseString(commentMessage._text);
		commentMessages.push_back(commentMessage);
	}

	void DbcParser::parseCommentNode(vector<DbcCommentNode> & commentNodes)
	{
		DbcCommentNode commentNode;
		this->_reader.parseIdentifier(commentNode._nodeName);
		this->_reader.parseString(commentNode._text);
		commentNodes.push_back(commentNode);
	}

	void DbcParser::parseCommentSignal(vector<DbcCommentSignal> & commentSignals)
	{
		DbcCommentSignal commentSignal;
		this->_reader.parseUInt32(commentSignal._messageId);
		this->_reader.parseIdentifier(commentSignal._signalName);
		this->_reader.parseString(commentSignal._text);
		commentSignals.push_back(commentSignal);
	}

	void DbcParser::parseEndOfLine()
	{
		this->_reader.matchToken(DbcTokenKind::EndOfLine);
		this->_reader.readNextToken();
		while (this->_reader.tryMatchToken(DbcTokenKind::EndOfLine))
		{
			this->_reader.readNextToken();
		}
	}

	void DbcParser::parseExtendedMultiplexing(vector<DbcExtendedMultiplexing> & extendedMultiplexing)
	{
		DbcExtendedMultiplexing item;
		this->_reader.matchKeyword(DbcKeyword::SG_MUL_VAL_);
		this->_reader.readNextToken();
		this->_reader.parseUInt32(item._messageId);
		this->_reader.parseIdentifier(item._multiplexedSignal);
		this->_reader.parseIdentifier(item._multiplexorSwitch);
		bool isFirst = true;
		while (!this->_reader.tryMatchToken(DbcTokenKind::OperatorSemicolon))
		{
			if (isFirst)
			{
				isFirst = false;
			}
			else
			{
				this->_reader.matchToken(DbcTokenKind::OperatorComma);
				this->_reader.readNextToken();
			}
			DbcMultiplexorValueRange range;
			this->_reader.parseUInt32(range._minimum);
			this->_reader.matchToken(DbcTokenKind::OperatorMinus);
			this->_reader.readNextToken();
			this->_reader.parseUInt32(range._maximum);
			item._multiplexorValueRanges.push_back(range);
		}
		this->_reader.readNextToken();
		this->parseEndOfLine();
		extendedMultiplexing.push_back(item);
	}

	void DbcParser::parseMessage(vector<DbcMessage> & messages)
	{
		DbcMessage item;
		this->_reader.matchKeyword(DbcKeyword::BO_);
		this->_reader.readNextToken();
		this->_reader.parseUInt32(item._id);
		this->_reader.parseIdentifier(item._name);
		this->_reader.matchToken(DbcTokenKind::OperatorColon);
		this->_reader.readNextToken();
		this->_reader.parseUInt32(item._size_byte);
		this->_reader.parseIdentifier(item._transmitter);
		this->parseEndOfLine();
		while (this->_reader.tryMatchKeyword(DbcKeyword::SG_))
		{
			this->parseSignal(item._signals);
		}
		messages.push_back(item);
	}

	void DbcParser::parseMessageTransmitters(vector<DbcMessageTransmitters> & transmitters)
	{
		DbcMessageTransmitters item;
		this->_reader.matchKeyword(DbcKeyword::BO_TX_BU_);
		this->_reader.readNextToken();
		this->_reader.parseUInt32(item._messageId);
		this->_reader.matchToken(DbcTokenKind::OperatorColon);
		this->_reader.readNextToken();
		bool checkNext = true;
		while (checkNext)
		{
			string id;
			this->_reader.parseIdentifier(id);
			item._transmitters.push_back(id);
			checkNext = this->_reader.tryMatchToken(DbcTokenKind::OperatorComma);
			if (checkNext)
			{
				this->_reader.readNextToken();
			}
		}
		this->_reader.matchToken(DbcTokenKind::OperatorSemicolon);
		this->_reader.readNextToken();
		this->parseEndOfLine();
		transmitters.push_back(item);
	}

	void DbcParser::parseNewSymbols(vector<string> & symbols)
	{
		this->_reader.matchKeyword(DbcKeyword::NS_);
		this->_reader.readNextToken();
		this->_reader.matchToken(DbcTokenKind::OperatorColon);
		this->_reader.readNextToken();
		this->parseEndOfLine();
		while (!this->_reader.tryMatchKeyword(DbcKeyword::BS_))
		{
			string id;
			this->_reader.parseIdentifier(id);
			symbols.push_back(id);
			this->parseEndOfLine();
		}
	}

	void DbcParser::parseNodes(vector<string> & nodes)
	{
		this->_reader.matchKeyword(DbcKeyword::BU_);
		this->_reader.readNextToken();
		this->_reader.matchToken(DbcTokenKind::OperatorColon);
		this->_reader.readNextToken();
		while (!this->_reader.tryMatchToken(DbcTokenKind::EndOfLine))
		{
			string id;
			this->_reader.parseIdentifier(id);
			nodes.push_back(id);
		}
		this->parseEndOfLine();
	}

	void DbcParser::parseSignal(vector<DbcSignal> & signalList)
	{
		DbcSignal item;
		this->_reader.matchKeyword(DbcKeyword::SG_);
		this->_reader.readNextToken();
		this->_reader.parseIdentifier(item._name);
		if (this->_reader.tryMatchToken(DbcTokenKind::OperatorColon))
		{
			item._multiplexorIndicators = DbcMultiplexorIndicator::None;
		}
		else
		{
			string id;
			this->_reader.parseIdentifier(id);
			if (id.back() == 'M')
			{
				item._multiplexorIndicators.setFlag(DbcMultiplexorIndicator::Multiplexor);
				id.erase(id.size() - 1, 1);
			}
			if (!id.empty() && id.front() == 'm')
			{
				item._multiplexorIndicators.setFlag(DbcMultiplexorIndicator::MultiplexedSignal);
				id.erase(0, 1);
				item._multiplexorSwitchValue = DbcTokenReader::literalIntegerTokenTextToUInt32(id);
				id.clear();
			}
			if (!id.empty())
			{
				throw DbcFormatException("Invalid multiplexer indicator");
			}
			this->_reader.matchToken(DbcTokenKind::OperatorColon);
		}
		this->_reader.readNextToken();
		this->_reader.parseUInt32(item._startBit);
		this->_reader.matchToken(DbcTokenKind::OperatorVerticalLine);
		this->_reader.readNextToken();
		this->parseSignalBase(item);
		{
			bool checkNext = true;
			while (checkNext)
			{
				string receiver;
				this->_reader.parseIdentifier(receiver);
				item._receivers.push_back(receiver);
				checkNext = this->_reader.tryMatchToken(DbcTokenKind::OperatorComma);
				if (checkNext)
				{
					this->_reader.readNextToken();
				}
			}
		}
		this->parseEndOfLine();
		signalList.push_back(item);
	}

	void DbcParser::parseSignalBase(DbcSignalBase & item)
	{
		this->_reader.parseUInt32(item._size_bit);
		this->_reader.matchToken(DbcTokenKind::OperatorCommercialAt);
		this->_reader.readNextToken();
		this->_reader.matchToken(DbcTokenKind::LiteralInteger);
		{
			const string & byteOrder  = this->_reader.token()._text;
			if (byteOrder == "0")
			{
				item._isBigEndian = true;
			}
			else if (byteOrder == "1")
			{
				item._isBigEndian = false;
			}
			else
			{
				throw DbcFormatException("Invalid byte order");
			}
		}
		this->_reader.readNextToken();
		if (this->_reader.tryMatchToken(DbcTokenKind::OperatorMinus))
		{
			item._isSigned = true;
		}
		else if (this->_reader.tryMatchToken(DbcTokenKind::OperatorPlus))
		{
			item._isSigned = false;
		}
		else
		{
			throw DbcFormatException("Missing value type");
		}
		this->_reader.readNextToken();
		this->_reader.matchToken(DbcTokenKind::OperatorLeftParenthesis);
		this->_reader.readNextToken();
		this->_reader.parseFloat64(item._factor);
		this->_reader.matchToken(DbcTokenKind::OperatorComma);
		this->_reader.readNextToken();
		this->_reader.parseFloat64(item._offset);
		this->_reader.matchToken(DbcTokenKind::OperatorRightParenthesis);
		this->_reader.readNextToken();
		this->_reader.matchToken(DbcTokenKind::OperatorLeftSquareBracket);
		this->_reader.readNextToken();
		this->_reader.parseFloat64(item._minimum);
		this->_reader.matchToken(DbcTokenKind::OperatorVerticalLine);
		this->_reader.readNextToken();
		this->_reader.parseFloat64(item._maximum);
		this->_reader.matchToken(DbcTokenKind::OperatorRightSquareBracket);
		this->_reader.readNextToken();
		this->_reader.parseString(item._unit);
	}

	void DbcParser::parseSignalGroup(vector<DbcSignalGroup> & groups)
	{
		DbcSignalGroup item;
		this->_reader.matchKeyword(DbcKeyword::SIG_GROUP_);
		this->_reader.readNextToken();
		this->_reader.parseUInt32(item._messageId);
		this->_reader.parseIdentifier(item._name);
		this->_reader.parseUInt32(item._repetitions);
		this->_reader.matchToken(DbcTokenKind::OperatorColon);
		this->_reader.readNextToken();
		while (!this->_reader.tryMatchToken(DbcTokenKind::OperatorSemicolon))
		{
			string name;
			this->_reader.parseIdentifier(name);
			item._signalNames.push_back(name);
		}
		this->_reader.readNextToken();
		this->parseEndOfLine();
		groups.push_back(item);
	}

	void DbcParser::parseSignalTypeOrReference(DbcDatabase & dbcDatabase)
	{
		DbcToken const token = this->_reader.token();
		this->_reader.matchKeyword(DbcKeyword::SGTYPE_);
		this->_reader.readNextToken();
		if (this->_reader.tryMatchToken(DbcTokenKind::Identifier))
		{
			DbcSignalType item;
			this->_reader.parseIdentifier(item._name);
			this->_reader.matchToken(DbcTokenKind::OperatorColon);
			this->_reader.readNextToken();
			this->parseSignalBase(item);
			this->_reader.parseFloat64(item._defaultValue);
			this->_reader.matchToken(DbcTokenKind::OperatorComma);
			this->_reader.readNextToken();
			this->_reader.parseIdentifier(item._valueTable);
			this->_reader.matchToken(DbcTokenKind::OperatorSemicolon);
			this->_reader.readNextToken();
			this->parseEndOfLine();
			dbcDatabase._signalTypes.push_back(item);
		}
		else if (this->_reader.tryMatchToken(DbcTokenKind::LiteralInteger))
		{
			DbcSignalTypeReference item;
			this->_reader.parseUInt32(item._messageId);
			this->_reader.parseIdentifier(item._signalName);
			this->_reader.matchToken(DbcTokenKind::OperatorColon);
			this->_reader.readNextToken();
			this->_reader.parseIdentifier(item._signalTypeName);
			this->_reader.matchToken(DbcTokenKind::OperatorSemicolon);
			this->_reader.readNextToken();
			this->parseEndOfLine();
			dbcDatabase._signalTypeReferences.push_back(item);
		}
		else
		{
			this->parseTailOfUnknownKeywordLine(token);
		}
	}

	void DbcParser::parseTailOfUnknownKeywordLine(const DbcToken & token)
	{
		if (this->_observer != nullptr)
		{
			this->_observer->onUnknownKeyword(token);
		}
		while (!this->_reader.tryMatchToken(DbcTokenKind::EndOfLine))
		{
			this->_reader.readNextToken();
		}
		this->parseEndOfLine();
	}

	void DbcParser::parseValueDescription(vector<DbcValueDescription> & descriptions)
	{
		DbcValueDescription entry;
		this->_reader.parseFloat64(entry._value);
		this->_reader.parseString(entry._description);
		descriptions.push_back(entry);
	}

	void DbcParser::parseValueDescriptions(DbcDatabase & dbcDatabase)
	{
		DbcToken const token = this->_reader.token();
		this->_reader.matchKeyword(DbcKeyword::VAL_);
		this->_reader.readNextToken();
		if (this->_reader.tryMatchToken(DbcTokenKind::LiteralInteger))
		{
			DbcSignalValueDescriptions item;
			this->_reader.parseUInt32(item._messageId);
			this->_reader.parseIdentifier(item._signalName);
			while (!this->_reader.tryMatchToken(DbcTokenKind::OperatorSemicolon))
			{
				this->parseValueDescription(item._descriptions);
			}
			this->_reader.readNextToken();
			this->parseEndOfLine();
			dbcDatabase._signalValueDescriptions.push_back(item);
		}
		else if (this->_reader.tryMatchToken(DbcTokenKind::Identifier))
		{
			DbcSignalEnvVarValueDescriptions item;
			this->_reader.parseIdentifier(item._envVarName);
			while (!this->_reader.tryMatchToken(DbcTokenKind::OperatorSemicolon))
			{
				this->parseValueDescription(item._descriptions);
			}
			this->_reader.readNextToken();
			this->parseEndOfLine();
			dbcDatabase._signalEnvVarValueDescriptions.push_back(item);
		}
		else
		{
			this->parseTailOfUnknownKeywordLine(token);
		}
	}

	void DbcParser::parseValueTable(vector<DbcValueTable> & tables)
	{
		DbcValueTable item;
		this->_reader.matchKeyword(DbcKeyword::VAL_TABLE_);
		this->_reader.readNextToken();
		this->_reader.parseIdentifier(item._name);
		while (!this->_reader.tryMatchToken(DbcTokenKind::OperatorSemicolon))
		{
			this->parseValueDescription(item._valueDescriptions);
		}
		this->_reader.readNextToken();
		this->parseEndOfLine();
		tables.push_back(item);
	}

	void DbcParser::parseVersion(string & version)
	{
		this->_reader.matchKeyword(DbcKeyword::VERSION);
		this->_reader.readNextToken();
		this->_reader.parseString(version);
		this->parseEndOfLine();
	}

	void DbcParser::setObserver(DbcParserObserver * observer)
	{
		this->_observer = observer;
	}

	DbcParserObserver::~DbcParserObserver()
	{
	}

	void DbcParserObserver::onUnknownKeyword(const DbcToken & /*token*/)
	{
	}

}
