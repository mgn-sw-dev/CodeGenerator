
#include <OptiScan/Parser/Dbc/DbcParser.h>
#include <OptiScan/Parser/Dbc/DbcFormatException.h>
#include <OptiScan/Parser/Dbc/DbcFormat.h>
#include <OptiScan/Parser/Dbc/DbcKeyword.h>

using namespace std;

namespace OptiScan::Parser::Dbc
{
	DbcParser::DbcParser(istream * input)
		: _scanner(input)
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

	bool DbcParser::tryMatchKeyword(const string & id)
	{
		bool result = false;
		if (this->tryMatchToken(DbcTokenKind::Identifier))
		{
			result = this->token()._text == id;
		}
		return result;
	}

	bool DbcParser::tryMatchToken(DbcTokenKind kind)
	{
		bool result = false;
		if (this->hasToken())
		{
			result = this->token()._kind == kind;
		}
		return result;
	}
}
