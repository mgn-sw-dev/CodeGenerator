
#include <OptiScan/Parser/Ldf/LdfKeyword.h>
#include <OptiScan/Parser/Ldf/LdfTokenReader.h>
#include <OptiScan/Parser/FileException.h>
#include <charconv>
#include <exception>

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

	uint32_t LdfTokenReader::literalHexIntegerTokenTextToUInt32(const string & tokenText)
	{
		if (tokenText.size() < 2 || tokenText[0] != '0' || (tokenText[1] != 'x' && tokenText[1] != 'X'))
		{
			throw FormatException("Invalid hex uint32: missing 0x prefix");
		}
		// pointer after '0x'
		char const * const first = tokenText.data() + 2;
		char const * const last = tokenText.data() + tokenText.size();
		uint32_t result = 0;
		from_chars_result const parseResult = from_chars(first, last, result, 16);
		bool const isValid = parseResult.ec == errc() && parseResult.ptr == last;
		if (!isValid)
		{
			throw FormatException("Invalid hex uint32");
		}
		return result;
	}

	uint32_t LdfTokenReader::literalIntegerTokenTextToUInt32(const string & tokenText)
	{
		uint32_t result = 0;
		char const * const first = tokenText.data();
		char const * const last = tokenText.data() + tokenText.size();
		from_chars_result const parseResult = from_chars(first, last, result);
		bool const isValid = parseResult.ec == errc() && parseResult.ptr == last;
		if (!isValid)
		{
			throw FormatException("Invalid uint32");
		}
		return result;
	}

	double LdfTokenReader::literalRealTokenTextToDouble(const string & tokenText)
	{
		double result = 0;
		char const * const first = tokenText.data();
		char const * const last = tokenText.data() + tokenText.size();
		from_chars_result const parseResult = from_chars(first, last, result);
		bool const isValid = parseResult.ec == errc() && parseResult.ptr == last;
		if (!isValid)
		{
			throw FormatException("Invalid double");
		}
		return result;
	}


	string LdfTokenReader::literalStringTokenTextToString(const string & tokenText)
	{
		string result;
		bool isValid = false;
		if (2 <= tokenText.size())
		{
			if (!tokenText.empty() && tokenText.front() == '"' && tokenText.back() == '"')
			{
				result = tokenText.substr(1, tokenText.size() - 2);
				isValid = result.find('"') == string::npos;
			}
		}
		if (!isValid)
		{
			throw FormatException("Invalid string");
		}
		return result;
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
			value = LdfTokenReader::literalHexIntegerTokenTextToUInt32(this->token()._text);
		}
		else if (this->tryMatchToken(LdfTokenKind::LiteralInteger) || this->tryMatchToken(LdfTokenKind::LiteralReal))
		{
			value = LdfTokenReader::literalRealTokenTextToDouble(this->token()._text);
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
		value = LdfTokenReader::literalStringTokenTextToString(this->token()._text);
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
			value = LdfTokenReader::literalHexIntegerTokenTextToUInt32(this->token()._text);
		}
		else if (this->tryMatchToken(LdfTokenKind::LiteralInteger))
		{
			value = LdfTokenReader::literalIntegerTokenTextToUInt32(this->token()._text);
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
