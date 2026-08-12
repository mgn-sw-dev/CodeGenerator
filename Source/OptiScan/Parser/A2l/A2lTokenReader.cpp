
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
}
