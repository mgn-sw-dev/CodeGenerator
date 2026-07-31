
#include <OptiScan/Parser/Dbc/DbcScanner.h>
#include <OptiScan/Parser/Dbc/DbcFormat.h>
#include <OptiScan/Parser/Dbc/DbcException.h>
#include <ios>

using namespace std;

namespace OptiScan::Parser::Dbc
{
	DbcScanPosition::DbcScanPosition()
		: _char(0)
		, _charInLine(0)
		, _line(0)
	{
	}

	DbcScanner::DbcScanner(std::istream * input)
		: _scanBuffer()
		, _stream(input)
		, _streamPosition()
		, _token()
	{
	}

	bool DbcScanner::fillScanBuffer(size_t count)
	{
		bool result = true;
		for (size_t i = this->_scanBuffer.size(); result && i < count; i++)
		{
			result = this->readCharFromStreamToScanBuffer();
		}
		return result;
	}

	bool DbcScanner::isCharDecimalSeparator(const char & c)
	{
		return c == '.';
	}

	bool DbcScanner::isCharDigit(const char & c)
	{
		return '0' <= c && c <= '9';
	}

	bool DbcScanner::isCharExponentStart(const char & c)
	{
		return c == 'e' || c == 'E';
	}

	bool DbcScanner::isCharIdentifier(const char & c)
	{
		return DbcScanner::isCharIdentifierStart(c) || DbcScanner::isCharDigit(c);
	}

	bool DbcScanner::isCharIdentifierStart(const char & c)
	{
		return false
			|| ('a' <= c && c <= 'z')
			|| ('A' <= c && c <= 'Z')
			|| c == '_'
		;
	}

	bool DbcScanner::isCharWhiteSpace(const char & c)
	{
		return false
			|| c == static_cast<char>(0x09)
			|| c == '\r'
			|| c == static_cast<char>(0x20)
		;
	}

	DbcScanChar DbcScanner::popScanBufferFront()
	{
		const DbcScanChar c = this->_scanBuffer.front();
		this->_scanBuffer.erase(this->_scanBuffer.begin());
		return c;
	}

	void DbcScanner::popScanBufferFrontToToken()
	{
		this->_token._text.push_back(this->_scanBuffer.front()._value);
		this->_scanBuffer.erase(this->_scanBuffer.begin());
	}

	bool DbcScanner::readCharFromStream(char & c)
	{
		bool result = false;
		int const value = this->_stream->get();
		if (value == std::char_traits<char>::eof())
		{
			if (this->_stream->bad())
			{
				throw ios_base::failure("Stream read error");
			}
			else
			{
				// normal stream end (eofbit/ failbit set but no I/O error)
			}
		}
		else
		{
			result = true;
			c = static_cast<char>(value);
			if (c == '\n')
			{
				this->_streamPosition._charInLine = 0;
				this->_streamPosition._line++;
			}
			else
			{
				this->_streamPosition._charInLine++;
			}
			this->_streamPosition._char++;
		}
		return result;
	}

	bool DbcScanner::readCharFromStreamToScanBuffer()
	{
		DbcScanChar c;
		c._position = this->_streamPosition;
		bool result = this->readCharFromStream(c._value);
		if (result)
		{
			this->_scanBuffer.push_back(c);
		}
		return result;
	}

	bool DbcScanner::readEndOfLine()
	{
		bool result = false;
		if (this->fillScanBuffer())
		{
			const DbcScanChar & c = this->_scanBuffer.front();
			if (c._value == '\n')
			{
				result = true;
				this->_token._kind = DbcTokenKind::EndOfLine;
				this->_token._position = c._position;
				this->popScanBufferFrontToToken();
			}
		}
		return result;
	}

	bool DbcScanner::readIdentifier()
	{
		bool result = false;
		if (this->fillScanBuffer())
		{
			const DbcScanChar & c = this->_scanBuffer.front();
			if (DbcScanner::isCharIdentifierStart(c._value))
			{
				result = true;
				this->_token._kind = DbcTokenKind::Identifier;
				this->_token._position = c._position;
				this->popScanBufferFrontToToken();
				bool checkNext = true;
				while (checkNext)
				{
					checkNext = this->fillScanBuffer();
					if (checkNext)
					{
						const DbcScanChar & c = this->_scanBuffer.front();
						checkNext = DbcScanner::isCharIdentifier(c._value);
						if (checkNext)
						{
							this->popScanBufferFrontToToken();
						}
					}
				}
			}
		}
		return result;
	}

	bool DbcScanner::readLiteralIntegerOrReal()
	{
		// Read digits.
		// On decimal separator or exponent start, literal real.
		// Otherwise, literal integer.
		bool result = false;
		if (this->fillScanBuffer())
		{
			size_t scanBufferIndex = 0;
			bool hasDecimalSeparator = DbcScanner::isCharDecimalSeparator(this->_scanBuffer.at(scanBufferIndex)._value);
			if (hasDecimalSeparator)
			{
				scanBufferIndex++;
			}
			if (this->fillScanBuffer(scanBufferIndex + 1))
			{
				if (DbcScanner::isCharDigit(this->_scanBuffer.at(scanBufferIndex)._value))
				{
					result = true;
					scanBufferIndex++;
					for (size_t i = 0; i < scanBufferIndex; i++)
					{
						const DbcScanChar c = this->popScanBufferFront();
						if (i == 0)
						{
							this->_token._position = c._position;
						}
						this->_token._text.push_back(c._value);
					}
					bool hasDecimalDigit = hasDecimalSeparator;
					bool hasExponent = false;
					bool hasExponentSign = false;
					bool hasExponentDigit = false;
					bool checkNext = true;
					while (checkNext)
					{
						checkNext = this->fillScanBuffer();
						if (checkNext)
						{
							const DbcScanChar & c = this->_scanBuffer.front();
							if (DbcScanner::isCharDecimalSeparator(c._value))
							{
								if (hasDecimalSeparator || hasExponent)
								{
									checkNext = false;
								}
								else
								{
									hasDecimalSeparator = true;
									this->popScanBufferFrontToToken();
								}
							}
							else if (DbcScanner::isCharDigit(c._value))
							{
								if (hasExponent)
								{
									hasExponentDigit = true;
								}
								else if (hasDecimalSeparator)
								{
									hasDecimalDigit = true;
								}
								this->popScanBufferFrontToToken();
							}
							else if (DbcScanner::isCharExponentStart(c._value))
							{
								if (hasExponent || (hasDecimalSeparator && !hasDecimalDigit))
								{
									checkNext = false;
								}
								else
								{
									hasExponent = true;
									this->popScanBufferFrontToToken();
								}
							}
							else if (c._value == '+' || c._value == '-')
							{
								if (!hasExponent || hasExponentSign || hasExponentDigit)
								{
									checkNext = false;
								}
								else
								{
									hasExponentSign = true;
									this->popScanBufferFrontToToken();
								}
							}
							else
							{
								checkNext = false;
							}
						}
					}

					if (hasDecimalSeparator && !hasDecimalDigit)
					{
						throw DbcFormatException("Missing decimal digit");
					}
					if (hasExponent && !hasExponentDigit)
					{
						throw DbcFormatException("Missing exponent digit");
					}

					if (hasDecimalSeparator || hasExponent)
					{
						this->_token._kind = DbcTokenKind::LiteralReal;
					}
					else
					{
						this->_token._kind = DbcTokenKind::LiteralInteger;
					}
				}
			}
		}
		return result;
	}

	bool DbcScanner::readLiteralString()
	{
		bool result = false;
		if (this->fillScanBuffer())
		{
			const DbcScanChar & c = this->_scanBuffer.front();
			if (c._value == '"')
			{
				result = true;
				this->_token._kind = DbcTokenKind::LiteralString;
				this->_token._position = c._position;
				this->popScanBufferFrontToToken();
				bool checkNext = true;
				while (checkNext)
				{
					if (!this->fillScanBuffer())
					{
						throw DbcFormatException("Missing quotation mark");
					}
					DbcScanChar & c = this->_scanBuffer.front();
					if (c._value == DbcFormat::StringEscapeStart)
					{
						this->popScanBufferFrontToToken();
						if (!this->fillScanBuffer())
						{
							throw DbcFormatException("Missing escape marker");
						}
						c = this->_scanBuffer.front();
						bool findMarker = true;
						for (size_t i = 0; findMarker && i < DbcFormat::StringEscapeItems.size(); i++)
						{
							findMarker = DbcFormat::StringEscapeItems[i]._escapeMarker != c._value;
						}
						if (findMarker)
						{
							throw DbcFormatException("Invalid escape marker");
						}
					}
					else
					{
						checkNext = c._value != '"';
					}
					this->popScanBufferFrontToToken();
				}
			}
		}
		return result;
	}

	bool DbcScanner::readOperator()
	{
		bool result = false;
		if (this->fillScanBuffer())
		{
			result = true;
			const DbcScanChar & c = this->_scanBuffer.front();
			if (c._value == ':')
			{
				this->_token._kind = DbcTokenKind::OperatorColon;
			}
			else if (c._value == ',')
			{
				this->_token._kind = DbcTokenKind::OperatorComma;
			}
			else if (c._value == '@')
			{
				this->_token._kind = DbcTokenKind::OperatorCommercialAt;
			}
			else if (c._value == '(')
			{
				this->_token._kind = DbcTokenKind::OperatorLeftParenthesis;
			}
			else if (c._value == '[')
			{
				this->_token._kind = DbcTokenKind::OperatorLeftSquareBracket;
			}
			else if (c._value == '-')
			{
				this->_token._kind = DbcTokenKind::OperatorMinus;
			}
			else if (c._value == '+')
			{
				this->_token._kind = DbcTokenKind::OperatorPlus;
			}
			else if (c._value == ')')
			{
				this->_token._kind = DbcTokenKind::OperatorRightParenthesis;
			}
			else if (c._value == ']')
			{
				this->_token._kind = DbcTokenKind::OperatorRightSquareBracket;
			}
			else if (c._value == ';')
			{
				this->_token._kind = DbcTokenKind::OperatorSemicolon;
			}
			else if (c._value == '|')
			{
				this->_token._kind = DbcTokenKind::OperatorVerticalLine;
			}
			else
			{
				result = false;
			}
			if (result)
			{
				this->_token._position = c._position;
				this->popScanBufferFrontToToken();
			}
		}
		return result;
	}

	void DbcScanner::scanNext()
	{
		this->_token = DbcToken();
		bool checkNext = true;
		while (checkNext)
		{
			checkNext = this->fillScanBuffer();
			if (checkNext)
			{
				checkNext = DbcScanner::isCharWhiteSpace(this->_scanBuffer.front()._value);
				if (checkNext)
				{
					this->_scanBuffer.erase(this->_scanBuffer.begin());
				}
			}
		}
		if (this->_scanBuffer.empty())
		{
			this->_token._position = this->_streamPosition;
		}
		else if (this->readEndOfLine())
		{
		}
		else if (this->readIdentifier())
		{
		}
		else if (this->readLiteralIntegerOrReal())
		{
		}
		else if (this->readLiteralString())
		{
		}
		else if (this->readOperator())
		{
		}
		else
		{
			const DbcScanChar & c = this->_scanBuffer.front();
			this->_token._position = c._position;
			this->_token._text.push_back(c._value);
			throw DbcFormatException("Unknown token");
		}
	}

	const DbcScanPosition & DbcScanner::streamPosition() const
	{
		return this->_streamPosition;
	}

	const DbcToken & DbcScanner::token() const
	{
		return this->_token;
	}

	DbcToken::DbcToken()
		: _kind(DbcTokenKind::None)
		, _position()
		, _text()
	{
	}

}
