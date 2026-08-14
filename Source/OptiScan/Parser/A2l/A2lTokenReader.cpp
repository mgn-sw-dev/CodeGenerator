
#include <OptiScan/Parser/A2l/A2lKeyword.h>
#include <OptiScan/Parser/A2l/A2lTokenReader.h>
#include <OptiScan/Parser/FileException.h>
#include <OptiScan/Parser/TokenReaderUtils.h>

using namespace std;

namespace OptiScan::Parser::A2l
{
	A2lTokenReader::A2lTokenReader(istream * input)
		: _scanner(input)
		, _tokenStack()
		, _tokenStackCount(-1)
	{
	}

	bool A2lTokenReader::hasToken() const
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

	void A2lTokenReader::matchKeyword(const string & id) const
	{
		if (!this->tryMatchKeyword(id))
		{
			throw FormatException("A2lTokenReader: Keyword mismatch");
		}
	}

	void A2lTokenReader::matchToken(A2lTokenKind kind) const
	{
		if (!this->tryMatchToken(kind))
		{
			throw FormatException("A2lTokenReader: Token mismatch");
		}
	}

	void A2lTokenReader::trackOneTimeKeyword(unordered_set<string> & oneTimeKeywords, const string & keyword)
	{
		if (oneTimeKeywords.find(keyword) != oneTimeKeywords.end())
		{
			throw FormatException("A2lTokenReader: Keyword already encountered");
		}
		oneTimeKeywords.insert(keyword);
	}

	void A2lTokenReader::parseFloat64(double & value)
	{
		if (this->tryMatchToken(A2lTokenKind::FloatLiteral) || this->tryMatchToken(A2lTokenKind::IntegerLiteral))
		{
			value = TokenReaderUtils::literalRealTokenTextToDouble(this->token()._text);
			this->readNextToken();
		}
		else
		{
			throw FormatException("A2lTokenReader: Float literal or integer literal expected");
		}
	}

	void A2lTokenReader::parseInt16(int16_t & value)
	{
		int64_t tmp;
		this->parseInt64(tmp);
		if (tmp < INT16_MIN || INT16_MAX < tmp)
		{
			throw FormatException("A2lTokenReader: Int16 out of range");
		}
		value = static_cast<int16_t>(tmp);
	}

	void A2lTokenReader::parseInt32(int32_t & value)
	{
		int64_t tmp;
		this->parseInt64(tmp);
		if (tmp < INT32_MIN || INT32_MAX < tmp)
		{
			throw FormatException("A2lTokenReader: Int32 out of range");
		}
		value = static_cast<int32_t>(tmp);
	}

	void A2lTokenReader::parseInt64(int64_t & value)
	{
		if (this->tryMatchToken(A2lTokenKind::HexLiteral))
		{
			uint64_t tmp = TokenReaderUtils::literalHexIntegerTokenTextToUInt64(this->token()._text);
			if (INT64_MAX < tmp)
			{
				throw FormatException("A2lTokenReader: Hex literal is not int64");
			}
			value = static_cast<int64_t>(tmp);
		}
		else if (this->tryMatchToken(A2lTokenKind::IntegerLiteral))
		{
			value = TokenReaderUtils::literalIntegerTokenTextToInt64(this->token()._text);
		}
		else
		{
			throw FormatException("A2lTokenReader: Hex literal or integer literal expected");
		}
		this->readNextToken();
	}

	void A2lTokenReader::parseString(string & value)
	{
		this->matchToken(A2lTokenKind::StringLiteral);
		// todo: process escape sequences
		value = TokenReaderUtils::literalStringTokenTextWithNestedStringToString(this->token()._text);
		this->readNextToken();
	}

	void A2lTokenReader::parseUInt8(uint8_t & value)
	{
		uint64_t tmp = 0;
		this->parseUInt64(tmp);
		if (UINT8_MAX < tmp)
		{
			throw FormatException("A2lTokenReader: UInt8 literal is not uint8");
		}
		value = static_cast<uint8_t>(tmp);
	}

	void A2lTokenReader::parseUInt16(uint16_t & value)
	{
		uint64_t tmp = 0;
		this->parseUInt64(tmp);
		if (UINT16_MAX < tmp)
		{
			throw FormatException("A2lTokenReader: UInt16 literal is not uint16");
		}
		value = static_cast<uint16_t>(tmp);
	}

	void A2lTokenReader::parseUInt32(uint32_t & value)
	{
		uint64_t tmp = 0;
		this->parseUInt64(tmp);
		if (UINT32_MAX < tmp)
		{
			throw FormatException("A2lTokenReader: UInt32 literal is not uint32");
		}
		value = static_cast<uint32_t>(tmp);
	}

	void A2lTokenReader::parseUInt64(uint64_t & value)
	{
		if (this->tryMatchToken(A2lTokenKind::HexLiteral))
		{
			value = TokenReaderUtils::literalHexIntegerTokenTextToUInt64(this->token()._text);
		}
		else if (this->tryMatchToken(A2lTokenKind::IntegerLiteral))
		{
			value = TokenReaderUtils::literalIntegerTokenTextToUInt64(this->token()._text);
		}
		else
		{
			throw FormatException("A2lTokenReader: Hex literal or integer literal expected");
		}
		this->readNextToken();
	}

	void A2lTokenReader::readNextToken()
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

	const A2lScanner & A2lTokenReader::scanner() const
	{
		return this->_scanner;
	}

	const A2lToken & A2lTokenReader::token() const
	{
		if (!this->hasToken())
		{
			throw InvalidOperationException("A2lTokenReader: No token");
		}
		const A2lToken * result;
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

	const vector<A2lToken> & A2lTokenReader::tokenStack() const
	{
		return this->_tokenStack;
	}

	void A2lTokenReader::tokenStackBegin()
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

	void A2lTokenReader::tokenStackCommit()
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

	int64_t A2lTokenReader::tokenStackCount() const
	{
		return this->_tokenStackCount;
	}

	void A2lTokenReader::tokenStackRollback()
	{
		if (this->_tokenStackCount == -1)
		{
			throw InvalidOperationException("Token stack rollback error: no token stack");
		}
		this->_tokenStackCount = -1;
	}

	bool A2lTokenReader::tryMatchKeyword(const string & id) const
	{
		bool result = false;
		if (this->tryMatchToken(A2lTokenKind::Identifier))
		{
			result = this->token()._text == id;
		}
		return result;
	}

	bool A2lTokenReader::tryMatchToken(A2lTokenKind kind) const
	{
		exception_ptr error;
		return this->tryMatchToken(kind, error);
	}

	bool A2lTokenReader::tryMatchToken(A2lTokenKind kind, exception_ptr & error) const
	{
		error = nullptr;
		bool result = this->hasToken();
		if (!result)
		{
			error = make_exception_ptr(InvalidOperationException("A2lTokenReader: No token"));
		}
		else
		{
			result = this->token()._kind == kind;
			if (!result)
			{
				error = make_exception_ptr(FormatException("A2lTokenReader: Token mismatch"));
			}
		}

		return result;
	}

	bool A2lTokenReader::tryParseUInt16(uint16_t & value, exception_ptr & error)
	{
		error = nullptr;
		bool result = false;
		this->tokenStackBegin();
		try
		{
			// check if A2lTokenKind::HexLiteral or A2lTokenKind::IntegerLiteral in parseUInt64 method
			this->parseUInt16(value);
			this->tokenStackCommit();
			result = true;
		}
		catch (const FormatException & e)
		{
			error = current_exception();
			this->tokenStackRollback();
			result = false;
		}
		return result;
	}

	bool A2lTokenReader::tryTokenStackBlockBegin(const string & keyword)
	{
		exception_ptr error;
		return this->tryTokenStackBlockBegin(keyword, error);
	}

	bool A2lTokenReader::tryTokenStackBlockBegin(const string & keyword, exception_ptr & error)
	{
		error = nullptr;
		string tmp;
		bool result = this->tryTokenStackBlockBeginAny(tmp, error);
		if (result)
		{
			result = keyword == tmp;
			if (!result)
			{
				error = make_exception_ptr(FormatException("A2lTokenReader: Keyword mismatch"));
			}
		}
		return result;
	}

	bool A2lTokenReader::tryTokenStackBlockBeginAny(string & keyword)
	{
		exception_ptr error;
		return this->tryTokenStackBlockBeginAny(keyword, error);
	}

	bool A2lTokenReader::tryTokenStackBlockBeginAny(string & keyword, exception_ptr & error)
	{
		error = nullptr;
		bool result = false;
		this->tokenStackBegin();
		try
		{
			if (this->tryMatchToken(A2lTokenKind::EscapeBegin, error))
			{
				this->readNextToken();
				if (this->tryMatchToken(A2lTokenKind::Identifier, error))
				{
					keyword = this->token()._text;
					result = true;
				}
			}
		}
		catch (const exception & e)
		{
			error = current_exception();
		}
		this->tokenStackRollback();
		return result;
	}
}
