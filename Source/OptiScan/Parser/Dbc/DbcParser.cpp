
#include <OptiScan/Parser/Dbc/DbcParser.h>
#include <OptiScan/Parser/Dbc/DbcException.h>
#include <OptiScan/Parser/Dbc/DbcFormat.h>
#include <OptiScan/Parser/Dbc/DbcKeyword.h>
#include <charconv>

using namespace std;

namespace OptiScan::Parser::Dbc
{
	DbcParser::DbcParser(istream * input)
		: _observer(nullptr)
		, _scanner(input)
		, _tokenStack()
		, _tokenStackCount(-1)
	{
	}

	bool DbcParser::hasToken() const
	{
		bool result;
		if (this->_tokenStackCount == -1)
		{
			result = 0 < this->_tokenStack.size();
		}
		else
		{
			result = 0 < this->_tokenStackCount && this->_tokenStackCount <= this->_tokenStack.size();
		}
		return result;
	}

	uint32_t DbcParser::literalIntegerTokenTextToUInt32(const string & tokenText)
	{
		uint32_t result = 0;
		char const * const first = tokenText.data();
		char const * const last = tokenText.data() + tokenText.size();
		from_chars_result const parseResult = from_chars(first, last, result);
		bool const isValid = parseResult.ec == errc() && parseResult.ptr == last;
		if (!isValid)
		{
			throw DbcFormatException("Invalid uint32");
		}
		return result;
	}

	double DbcParser::literalRealTokenTextToDouble(const std::string & tokenText)
	{
		double result = 0;
		char const * const first = tokenText.data();
		char const * const last = tokenText.data() + tokenText.size();
		from_chars_result const parseResult = from_chars(first, last, result);
		bool const isValid = parseResult.ec == errc() && parseResult.ptr == last;
		if (!isValid)
		{
			throw DbcFormatException("Invalid double");
		}
		return result;
	}

	string DbcParser::literalStringTokenTextToString(const string & tokenText)
	{
		string result;
		bool isValid = false;
		if (2 <= tokenText.size())
		{
			if (!tokenText.empty() && tokenText.front() == '"' && tokenText.back() == '"')
			{
				isValid = true;
				bool isEscape = false;
				for (size_t i = 1; isValid && i < tokenText.size() - 1; i++)
				{
					char const c = tokenText[i];
					if (isEscape)
					{
						for (size_t itemIndex = 0; isEscape && itemIndex < DbcFormat::StringEscapeItems.size(); itemIndex++)
						{
							const DbcStringEscapeItem & item = DbcFormat::StringEscapeItems[itemIndex];
							if (item._escapeMarker == c)
							{
								result.push_back(item._escapedChar);
								isEscape = false;
							}
						}
						if (isEscape)
						{
							isValid = false;
						}
					}
					else if (c == DbcFormat::StringEscapeStart)
					{
						isEscape = true;
					}
					else
					{
						result.push_back(c);
					}
				}
				isValid = isValid && !isEscape;
			}
		}
		if (!isValid)
		{
			throw DbcFormatException("Invalid string");
		}
		return result;
	}

	void DbcParser::matchKeyword(const string & id)
	{
		if (!this->tryMatchKeyword(id))
		{
			throw DbcFormatException("Keyword mismatch");
		}
	}

	void DbcParser::matchToken(DbcTokenKind kind)
	{
		if (!this->tryMatchToken(kind))
		{
			throw DbcFormatException("Token mismatch");
		}
	}

	void DbcParser::parseAttribute(DbcDatabase & dbcDatabase)
	{
		DbcToken const token = this->token();
		this->matchKeyword(DbcKeyword::BA_);
		this->readNextToken();
		string name;
		this->parseString(name);
		bool hasValue = true;
		if (!this->tryMatchToken(DbcTokenKind::Identifier))
		{
			DbcAttribute item;
			item._name = name;
			this->parseAttributeValue(item._value);
			dbcDatabase._attributes.push_back(item);
		}
		else if (this->tryMatchKeyword(DbcKeyword::BO_))
		{
			this->readNextToken();
			DbcAttributeMessage item;
			item._name = name;
			this->parseUInt32(item._messageId);
			this->parseAttributeValue(item._value);
			dbcDatabase._attributeMessages.push_back(item);
		}
		else if (this->tryMatchKeyword(DbcKeyword::BU_))
		{
			this->readNextToken();
			DbcAttributeNode item;
			item._name = name;
			this->parseIdentifier(item._nodeName);
			this->parseAttributeValue(item._value);
			dbcDatabase._attributeNodes.push_back(item);
		}
		else if (this->tryMatchKeyword(DbcKeyword::EV_))
		{
			this->readNextToken();
			DbcAttributeEnvVar item;
			item._name = name;
			this->parseIdentifier(item._envVarName);
			this->parseAttributeValue(item._value);
			dbcDatabase._attributeEnvVars.push_back(item);
		}
		else if (this->tryMatchKeyword(DbcKeyword::SG_))
		{
			this->readNextToken();
			DbcAttributeSignal item;
			item._name = name;
			this->parseUInt32(item._messageId);
			this->parseIdentifier(item._signalName);
			this->parseAttributeValue(item._value);
			dbcDatabase._attributeSignals.push_back(item);
		}
		else
		{
			hasValue = false;
		}
		if (hasValue)
		{
			this->matchToken(DbcTokenKind::OperatorSemicolon);
			this->readNextToken();
			this->parseEndOfLine();
		}
		else
		{
			this->parseTailOfUnknownKeywordLine(token);
		}
	}

	void DbcParser::parseAttributeValue(DbcAttributeValue & value)
	{
		if (this->tryMatchToken(DbcTokenKind::Identifier))
		{
			string tmp;
			this->parseIdentifier(tmp);
			value = tmp;
		}
		else if(this->tryMatchToken(DbcTokenKind::LiteralString))
		{
			string tmp;
			this->parseString(tmp);
			value = tmp;
		}
		else
		{
			exception_ptr error;
			{
				uint32_t tmp;
				error = this->tryParseUInt32(tmp);
				if (!error)
				{
					value = tmp;
				}
			}
			if (error)
			{
				int32_t tmp;
				error = this->tryParseInt32(tmp);
				if (!error)
				{
					value = tmp;
				}
			}
			if (error)
			{
				double tmp;
				this->parseFloat64(tmp);
				value = tmp;
			}
		}
	}

	void DbcParser::parse(DbcDatabase & dbcDatabase)
	{
		dbcDatabase = DbcDatabase();

		this->readNextToken();
		if (this->tryMatchToken(DbcTokenKind::EndOfLine))
		{
			this->parseEndOfLine();
		}

		if (this->tryMatchKeyword(DbcKeyword::VERSION))
		{
			this->parseVersion(dbcDatabase._version);
		}

		if (this->tryMatchKeyword(DbcKeyword::NS_))
		{
			this->parseNewSymbols(dbcDatabase._newSymbols);
		}

		this->parseBitTiming(dbcDatabase._bitTiming);

		while (!this->tryMatchToken(DbcTokenKind::None))
		{
			this->matchToken(DbcTokenKind::Identifier);
			const string & keyword = this->token()._text;
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
				break;
			case DbcKeywordKind::BA_DEF_DEF_:
				break;
			case DbcKeywordKind::BO_:
				break;
			case DbcKeywordKind::BO_TX_BU_:
				break;
			case DbcKeywordKind::BS_:
				break;
			case DbcKeywordKind::BU_:
				break;
			case DbcKeywordKind::CM_:
				break;
			case DbcKeywordKind::EV_:
				break;
			case DbcKeywordKind::NS_:
				break;
			case DbcKeywordKind::SG_:
				break;
			case DbcKeywordKind::SG_MUL_VAL_:
				break;
			case DbcKeywordKind::SGTYPE_:
				break;
			case DbcKeywordKind::VAL_:
				break;
			case DbcKeywordKind::VAL_TABLE_:
				break;
			case DbcKeywordKind::VERSION:
				break;
			case DbcKeywordKind::Unknown:
				break;
			}
		}
	}

	void DbcParser::parseBitTiming(DbcBitTiming & bitTiming)
	{
		this->matchKeyword(DbcKeyword::BS_);
		this->readNextToken();
		this->matchToken(DbcTokenKind::OperatorColon);
		this->readNextToken();
		if (!this->tryMatchToken(DbcTokenKind::EndOfLine))
		{
			this->parseUInt32(bitTiming._baudrate);
			this->matchToken(DbcTokenKind::OperatorColon);
			this->readNextToken();
			this->parseUInt32(bitTiming._btr1);
			this->matchToken(DbcTokenKind::OperatorComma);
			this->readNextToken();
			this->parseUInt32(bitTiming._btr2);
		}
		this->parseEndOfLine();
	}

	void DbcParser::parseEndOfLine()
	{
		this->matchToken(DbcTokenKind::EndOfLine);
		this->readNextToken();
		while (this->tryMatchToken(DbcTokenKind::EndOfLine))
		{
			this->readNextToken();
		}
	}

	void DbcParser::parseFloat64(double & value)
	{
		bool isSigned = false;
		if (this->tryMatchToken(DbcTokenKind::OperatorMinus))
		{
			isSigned = true;
			this->readNextToken();
		}
		else if (this->tryMatchToken(DbcTokenKind::OperatorPlus))
		{
			this->readNextToken();
		}
		double tmp;
		if (this->tryMatchToken(DbcTokenKind::LiteralInteger) || this->tryMatchToken(DbcTokenKind::LiteralReal))
		{
			tmp = DbcParser::literalRealTokenTextToDouble(this->token()._text);
		}
		else
		{
			throw DbcFormatException("literal integer or literal real expected");
		}
		if (isSigned)
		{
			tmp = -tmp;
		}
		value = tmp;
		this->readNextToken();
	}

	void DbcParser::parseIdentifier(string & value)
	{
		this->matchToken(DbcTokenKind::Identifier);
		value = this->token()._text;
		this->readNextToken();
	}

	void DbcParser::parseNewSymbols(vector<string> & symbols)
	{
		this->matchKeyword(DbcKeyword::NS_);
		this->readNextToken();
		this->matchToken(DbcTokenKind::OperatorColon);
		this->readNextToken();
		this->parseEndOfLine();
		while (!this->tryMatchKeyword(DbcKeyword::BS_))
		{
			string id;
			this->parseIdentifier(id);
			symbols.push_back(id);
			this->parseEndOfLine();
		}
	}

	void DbcParser::parseString(string & value)
	{
		this->matchToken(DbcTokenKind::LiteralString);
		value = DbcParser::literalStringTokenTextToString(this->token()._text);
		this->readNextToken();
	}

	void DbcParser::parseTailOfUnknownKeywordLine(const DbcToken & token)
	{
		if (this->_observer != nullptr)
		{
			this->_observer->onUnknownKeyword(token);
		}
		while (!this->tryMatchToken(DbcTokenKind::EndOfLine))
		{
			this->readNextToken();
		}
		this->parseEndOfLine();
	}

	void DbcParser::parseUInt32(uint32_t & value)
	{
		exception_ptr const error = this->tryParseUInt32(value);
		if (error)
		{
			rethrow_exception(error);
		}
	}
	void DbcParser::parseVersion(string & version)
	{
		this->matchKeyword(DbcKeyword::VERSION);
		this->readNextToken();
		this->parseString(version);
		this->parseEndOfLine();
	}

	void DbcParser::readNextToken()
	{
		if (this->_tokenStackCount == -1)
		{
			if (0 < this->_tokenStack.size())
			{
				this->_tokenStack.erase(this->_tokenStack.begin());
			}
			if (0 == this->_tokenStack.size())
			{
				this->_scanner.scanNext();
				this->_tokenStack.push_back(this->_scanner.token());
			}
		}
		else
		{
			if (this->_tokenStack.size() <= this->_tokenStackCount)
			{
				this->_scanner.scanNext();
				this->_tokenStack.push_back(this->_scanner.token());
			}
			this->_tokenStackCount++;
		}
	}

	const DbcToken & DbcParser::token() const
	{
		if (!this->hasToken())
		{
			// ToDo: which exception?
			throw runtime_error("No token");
		}
		const DbcToken * result;
		if (this->_tokenStackCount == -1)
		{
			result = &this->_tokenStack[0];
		}
		else
		{
			result = &this->_tokenStack[this->_tokenStackCount - 1];
		}
		return *result;
	}

	void DbcParser::tokenStackBegin()
	{
		if (this->_tokenStackCount != -1)
		{
			throw DbcInvalidOperationException("Token stack begin error: token stack already in use");
		}
		if (this->_tokenStack.size() < 1)
		{
			this->_tokenStackCount = 0;
		}
		else
		{
			this->_tokenStackCount = 1;
		}
	}

	void DbcParser::tokenStackCommit()
	{
		if (this->_tokenStackCount == -1)
		{
			throw DbcInvalidOperationException("Token stack commit error: no token stack");
		}
		for (size_t i = 0; i < this->_tokenStackCount; i++)
		{
			this->_tokenStack.erase(this->_tokenStack.begin());
		}
		this->_tokenStackCount = -1;
	}

	void DbcParser::tokenStackRollback()
	{
		if (this->_tokenStackCount == -1)
		{
			throw DbcInvalidOperationException("Token stack rollback error: no token stack");
		}
		this->_tokenStackCount = -1;
	}

	bool DbcParser::tryMatchKeyword(const string & id)
	{
		bool result = false;
		if (this->tryMatchToken(DbcTokenKind::Identifier))
		{
			result = this->token()._text == id;
		}
		return result;
	}

	bool DbcParser::tryMatchToken(DbcTokenKind kind) const
	{
		bool result = false;
		if (this->hasToken())
		{
			result = this->token()._kind == kind;
		}
		return result;
	}

	exception_ptr DbcParser::tryParseInt32(int32_t & value)
	{
		exception_ptr result;
		this->tokenStackBegin();
		try
		{
			bool isSigned = false;
			if (this->tryMatchToken(DbcTokenKind::OperatorMinus))
			{
				isSigned = true;
				this->readNextToken();
			}
			else if (this->tryMatchToken(DbcTokenKind::OperatorPlus))
			{
				this->readNextToken();
			}
			this->matchToken(DbcTokenKind::LiteralInteger);
			uint32_t const raw = DbcParser::literalIntegerTokenTextToUInt32(this->token()._text);
			int64_t signedValue = static_cast<int64_t>(raw);
			if (isSigned)
			{
				signedValue = -signedValue;
			}
			if (signedValue < INT32_MIN || INT32_MAX < signedValue)
			{
				throw DbcFormatException("Int32 out of range");
			}
			value = static_cast<int32_t>(signedValue);
			this->tokenStackCommit();
			this->readNextToken();
		}
		catch (const DbcFormatException & ex)
		{
			result = current_exception();
			this->tokenStackRollback();
		}
		return result;
	}

	exception_ptr DbcParser::tryParseUInt32(uint32_t & value)
	{
		exception_ptr result;
		this->tokenStackBegin();
		try
		{
			if (this->tryMatchToken(DbcTokenKind::OperatorPlus))
			{
				this->readNextToken();
			}
			this->matchToken(DbcTokenKind::LiteralInteger);
			value = DbcParser::literalIntegerTokenTextToUInt32(this->token()._text);
			this->tokenStackCommit();
			this->readNextToken();
		}
		catch (const DbcFormatException & ex)
		{
			// catch only DbcFormatException and not DbcInvalidOperationException
			result = current_exception();
			this->tokenStackRollback();
		}
		return result;
	}

	DbcParserObserver::~DbcParserObserver()
	{
	}

	void DbcParserObserver::onUnknownKeyword(const DbcToken & /*token*/)
	{
	}

}
