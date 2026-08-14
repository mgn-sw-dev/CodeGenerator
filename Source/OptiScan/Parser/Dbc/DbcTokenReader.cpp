
#include <OptiScan/Parser/Dbc/DbcFormat.h>
#include <OptiScan/Parser/Dbc/DbcTokenReader.h>
#include <OptiScan/Parser/FileException.h>
#include <OptiScan/Parser/TokenReaderUtils.h>
#include <exception>

using namespace std;

namespace OptiScan::Parser::Dbc
{
	DbcTokenReader::DbcTokenReader(istream * input)
		: _scanner(input)
		, _tokenStack()
		, _tokenStackCount(-1)
	{
	}

	bool DbcTokenReader::hasToken() const
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

	string DbcTokenReader::literalStringTokenTextToString(const string & tokenText)
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
			throw FormatException("Invalid string");
		}
		return result;
	}

	void DbcTokenReader::matchKeyword(const string & id) const
	{
		if (!this->tryMatchKeyword(id))
		{
			throw FormatException("Keyword mismatch");
		}
	}

	void DbcTokenReader::matchToken(DbcTokenKind kind) const
	{
		if (!this->tryMatchToken(kind))
		{
			throw FormatException("Token mismatch");
		}
	}

	void DbcTokenReader::parseFloat64(double & value)
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
			tmp = TokenReaderUtils::literalRealTokenTextToDouble(this->token()._text);
		}
		else
		{
			throw FormatException("literal integer or literal real expected");
		}
		if (isSigned)
		{
			tmp = -tmp;
		}
		value = tmp;
		this->readNextToken();
	}

	void DbcTokenReader::parseIdentifier(string & value)
	{
		this->matchToken(DbcTokenKind::Identifier);
		value = this->token()._text;
		this->readNextToken();
	}

	void DbcTokenReader::parseInt32(int32_t & value)
	{
		exception_ptr const error = this->tryParseInt32(value);
		if (error)
		{
			rethrow_exception(error);
		}
	}

	void DbcTokenReader::parseString(string & value)
	{
		this->matchToken(DbcTokenKind::LiteralString);
		value = DbcTokenReader::literalStringTokenTextToString(this->token()._text);
		this->readNextToken();
	}

	void DbcTokenReader::parseUInt32(uint32_t & value)
	{
		exception_ptr const error = this->tryParseUInt32(value);
		if (error)
		{
			rethrow_exception(error);
		}
	}

	void DbcTokenReader::readNextToken()
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

	const DbcScanner & DbcTokenReader::scanner() const
	{
		return this->_scanner;
	}

	const DbcToken & DbcTokenReader::token() const
	{
		if (!this->hasToken())
		{
			throw InvalidOperationException("No token");
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

	const vector<DbcToken> & DbcTokenReader::tokenStack() const
	{
		return this->_tokenStack;
	}

	void DbcTokenReader::tokenStackBegin()
	{
		if (this->_tokenStackCount != -1)
		{
			throw InvalidOperationException("Token stack begin error: token stack already in use");
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

	void DbcTokenReader::tokenStackCommit()
	{
		if (this->_tokenStackCount == -1)
		{
			throw InvalidOperationException("Token stack commit error: no token stack");
		}
		for (int64_t i = 0; i < this->_tokenStackCount; i++)
		{
			this->_tokenStack.erase(this->_tokenStack.begin());
		}
		this->_tokenStackCount = -1;
	}

	int64_t DbcTokenReader::tokenStackCount() const
	{
		return this->_tokenStackCount;
	}

	void DbcTokenReader::tokenStackRollback()
	{
		if (this->_tokenStackCount == -1)
		{
			throw InvalidOperationException("Token stack rollback error: no token stack");
		}
		this->_tokenStackCount = -1;
	}

	bool DbcTokenReader::tryMatchKeyword(const string & id) const
	{
		bool result = false;
		if (this->tryMatchToken(DbcTokenKind::Identifier))
		{
			result = this->token()._text == id;
		}
		return result;
	}

	bool DbcTokenReader::tryMatchToken(DbcTokenKind kind) const
	{
		bool result = false;
		if (this->hasToken())
		{
			result = this->token()._kind == kind;
		}
		return result;
	}

	bool DbcTokenReader::tryParseInt32(int32_t & value, exception_ptr & error)
	{
		error = nullptr;
		bool result = true;
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
			uint32_t const raw = TokenReaderUtils::literalIntegerTokenTextToUInt32(this->token()._text);
			int64_t signedValue = static_cast<int64_t>(raw);
			if (isSigned)
			{
				signedValue = -signedValue;
			}
			if (signedValue < INT32_MIN || INT32_MAX < signedValue)
			{
				throw FormatException("Int32 out of range");
			}
			value = static_cast<int32_t>(signedValue);
			this->tokenStackCommit();
			this->readNextToken();
			result = true;
		}
		catch (const FormatException & ex)
		{
			error = current_exception();
			this->tokenStackRollback();
			result = false;
		}
		return result;
	}

	bool DbcTokenReader::tryParseUInt32(uint32_t & value, exception_ptr & error)
	{
		error = nullptr;
		bool result = true;
		this->tokenStackBegin();
		try
		{
			if (this->tryMatchToken(DbcTokenKind::OperatorPlus))
			{
				this->readNextToken();
			}
			this->matchToken(DbcTokenKind::LiteralInteger);
			value = TokenReaderUtils::literalIntegerTokenTextToUInt32(this->token()._text);
			this->tokenStackCommit();
			this->readNextToken();
			result = true;
		}
		catch (const FormatException & ex)
		{
			// catch only FormatException and not InvalidOperationException
			error = current_exception();
			this->tokenStackRollback();
			result = false;
		}
		return result;
	}
}
