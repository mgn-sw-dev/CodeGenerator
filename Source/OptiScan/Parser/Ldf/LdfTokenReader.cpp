
#include <OptiScan/Parser/Ldf/LdfKeyword.h>
#include <OptiScan/Parser/Ldf/LdfTokenReader.h>
#include <OptiScan/Parser/FileException.h>
#include <OptiScan/Parser/TokenReaderUtils.h>

using namespace std;

namespace OptiScan::Parser::Ldf
{
	LdfTokenReader::LdfTokenReader(std::istream * input)
		: _hasToken(false)
		, _scanner(input)
	{
	}

	bool LdfTokenReader::hasToken() const
	{
		return this->_hasToken;
	}

	void LdfTokenReader::matchKeyword(const string & id) const
	{
		if (!this->tryMatchKeyword(id))
		{
			throw FormatException("Keyword mismatch");
		}
	}

	void LdfTokenReader::matchKeywordAndRead(const string & id)
	{
		this->matchKeyword(id);
		this->readNextToken();
	}

	void LdfTokenReader::matchToken(const LdfTokenKind kind) const
	{
		if (!this->tryMatchToken(kind))
		{
			throw FormatException("Token mismatch");
		}
	}

	void LdfTokenReader::matchTokenAndRead(LdfTokenKind kind)
	{
		this->matchToken(kind);
		this->readNextToken();
	}

	void LdfTokenReader::parseFloat64(double & value)
	{
		if (this->tryMatchToken(LdfTokenKind::LiteralHexInteger))
		{
			value = TokenReaderUtils::literalHexIntegerTokenTextToUInt32(this->token()._text);
		}
		else if (this->tryMatchToken(LdfTokenKind::LiteralInteger) || this->tryMatchToken(LdfTokenKind::LiteralReal))
		{
			value = TokenReaderUtils::literalRealTokenTextToDouble(this->token()._text);
		}
		else
		{
			throw FormatException("Literal hex integer, literal integer or literal real expected");
		}
		this->readNextToken();
	}

	void LdfTokenReader::parseFloat64_ms(double & value_ms)
	{
		this->parseFloat64(value_ms);
		this->matchKeywordAndRead(LdfKeyword::Ms);
	}

	void LdfTokenReader::parseIdentifier(string & value)
	{
		this->matchToken(LdfTokenKind::Identifier);
		value = this->token()._text;
		this->readNextToken();
	}

	void LdfTokenReader::parseString(string & value)
	{
		this->matchToken(LdfTokenKind::LiteralString);
		value = TokenReaderUtils::literalStringTokenTextToString(this->token()._text);
		this->readNextToken();
	}

	void LdfTokenReader::parseUInt8(uint8_t & value)
	{
		uint32_t tmp;
		this->parseUInt32(tmp);
		if (UINT8_MAX < tmp)
		{
			throw FormatException("UInt8 out of range");
		}
		value = static_cast<uint8_t>(tmp);
	}

	void LdfTokenReader::parseUInt16(uint16_t & value)
	{
		uint32_t tmp;
		this->parseUInt32(tmp);
		if (UINT16_MAX < tmp)
		{
			throw FormatException("UInt16 out of range");
		}
		value = static_cast<uint16_t>(tmp);
	}

	void LdfTokenReader::parseUInt32(uint32_t & value)
	{
		if (this->tryMatchToken(LdfTokenKind::LiteralHexInteger))
		{
			value = TokenReaderUtils::literalHexIntegerTokenTextToUInt32(this->token()._text);
		}
		else if (this->tryMatchToken(LdfTokenKind::LiteralInteger))
		{
			value = TokenReaderUtils::literalIntegerTokenTextToUInt32(this->token()._text);
		}
		else
		{
			throw FormatException("Literal hex integer or literal integer expected");
		}
		this->readNextToken();
	}

	void LdfTokenReader::readNextToken()
	{
		this->_scanner.scanNext();
		this->_hasToken = true;
	}

	const LdfToken & LdfTokenReader::token() const
	{
		if (!this->hasToken())
		{
			throw InvalidOperationException("No token");
		}
		return this->_scanner.token();
	}

	bool LdfTokenReader::tryMatchKeyword(const string & id) const
	{
		bool result = false;
		if (this->tryMatchToken(LdfTokenKind::Identifier))
		{
			result = this->token()._text == id;
		}
		return result;
	}

	bool LdfTokenReader::tryMatchToken(LdfTokenKind kind) const
	{
		bool result = false;
		if (this->hasToken())
		{
			result = this->token()._kind == kind;
		}
		return result;
	}
}
