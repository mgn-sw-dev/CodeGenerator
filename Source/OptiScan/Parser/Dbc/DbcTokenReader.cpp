
#include <OptiScan/Parser/Dbc/DbcException.h>
#include <OptiScan/Parser/Dbc/DbcFormat.h>
#include <OptiScan/Parser/Dbc/DbcTokenReader.h>
#include <charconv>
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

	uint32_t DbcTokenReader::literalIntegerTokenTextToUInt32(const string & tokenText)
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

	double DbcTokenReader::literalRealTokenTextToDouble(const std::string & tokenText)
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
			throw DbcFormatException("Invalid string");
		}
		return result;
	}

	void DbcTokenReader::matchKeyword(const string & id) const
	{
		if (!this->tryMatchKeyword(id))
		{
			throw DbcFormatException("Keyword mismatch");
		}
	}

	void DbcTokenReader::matchToken(DbcTokenKind kind) const
	{
		if (!this->tryMatchToken(kind))
		{
			throw DbcFormatException("Token mismatch");
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
			tmp = DbcTokenReader::literalRealTokenTextToDouble(this->token()._text);
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
			throw DbcInvalidOperationException("No token");
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

	void DbcTokenReader::tokenStackCommit()
	{
		if (this->_tokenStackCount == -1)
		{
			throw DbcInvalidOperationException("Token stack commit error: no token stack");
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
			throw DbcInvalidOperationException("Token stack rollback error: no token stack");
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

	exception_ptr DbcTokenReader::tryParseInt32(int32_t & value)
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
			uint32_t const raw = DbcTokenReader::literalIntegerTokenTextToUInt32(this->token()._text);
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

	exception_ptr DbcTokenReader::tryParseUInt32(uint32_t & value)
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
			value = DbcTokenReader::literalIntegerTokenTextToUInt32(this->token()._text);
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
}
