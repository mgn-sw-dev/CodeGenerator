
#include <OptiScan/Parser/Dbc/DbcScanner.h>
#include <OptiScan/Parser/Dbc/DbcFormat.h>
#include <OptiScan/Parser/Dbc/DbcException.h>
#include <ios>

using namespace std;

namespace OptiScan::Parser::Dbc
{
	DbcScanner::DbcScanner(std::istream * input)
		: _reader(input)
		, _token()
	{
	}

	bool DbcScanner::isCharDecimalSeparator(const char & c)
	{
		return c == '.';
	}

	bool DbcScanner::isCharExponentStart(const char & c)
	{
		return c == 'e' || c == 'E';
	}

	bool DbcScanner::isCharWhiteSpace(const char & c)
	{
		return false
			|| c == static_cast<char>(0x09)
			|| c == '\r'
			|| c == static_cast<char>(0x20)
		;
	}

	void DbcScanner::popScanBufferFrontToToken()
	{
		ScanChar const c = this->_reader.popBufferFront();
		this->_token._text.push_back(c._value);
	}

	bool DbcScanner::readEndOfLine()
	{
		bool result = false;
		if (this->_reader.fillScanBuffer())
		{
			const ScanChar & c = this->_reader.front();
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
		if (this->_reader.fillScanBuffer())
		{
			const ScanChar & c = this->_reader.front();
			if (CharReader::isCharIdentifierStart(c._value))
			{
				result = true;
				this->_token._kind = DbcTokenKind::Identifier;
				this->_token._position = c._position;
				this->popScanBufferFrontToToken();
				bool checkNext = true;
				while (checkNext)
				{
					checkNext = this->_reader.fillScanBuffer();
					if (checkNext)
					{
						const ScanChar & c = this->_reader.front();
						checkNext = CharReader::isCharIdentifier(c._value);
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
		if (this->_reader.fillScanBuffer())
		{
			size_t scanBufferIndex = 0;
			bool hasDecimalSeparator = DbcScanner::isCharDecimalSeparator(this->_reader.at(scanBufferIndex)._value);
			if (hasDecimalSeparator)
			{
				scanBufferIndex++;
			}
			if (this->_reader.fillScanBuffer(scanBufferIndex + 1))
			{
				if (CharReader::isCharDigit(this->_reader.at(scanBufferIndex)._value))
				{
					result = true;
					scanBufferIndex++;
					for (size_t i = 0; i < scanBufferIndex; i++)
					{
						ScanChar const c = this->_reader.popBufferFront();
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
						checkNext = this->_reader.fillScanBuffer();
						if (checkNext)
						{
							const ScanChar & c = this->_reader.front();
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
							else if (CharReader::isCharDigit(c._value))
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
		if (this->_reader.fillScanBuffer())
		{
			const ScanChar & c = this->_reader.front();
			if (c._value == '"')
			{
				result = true;
				this->_token._kind = DbcTokenKind::LiteralString;
				this->_token._position = c._position;
				this->popScanBufferFrontToToken();
				bool checkNext = true;
				while (checkNext)
				{
					if (!this->_reader.fillScanBuffer())
					{
						throw DbcFormatException("Missing quotation mark");
					}
					const ScanChar & c = this->_reader.front();
					if (c._value == DbcFormat::StringEscapeStart)
					{
						this->popScanBufferFrontToToken();
						if (!this->_reader.fillScanBuffer())
						{
							throw DbcFormatException("Missing escape marker");
						}
						const ScanChar & escaped = this->_reader.front();
						bool findMarker = true;
						for (size_t i = 0; findMarker && i < DbcFormat::StringEscapeItems.size(); i++)
						{
							findMarker = DbcFormat::StringEscapeItems[i]._escapeMarker != escaped._value;
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
		if (this->_reader.fillScanBuffer())
		{
			result = true;
			const ScanChar & c = this->_reader.front();
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
			checkNext = this->_reader.fillScanBuffer();
			if (checkNext)
			{
				checkNext = DbcScanner::isCharWhiteSpace(this->_reader.front()._value);
				if (checkNext)
				{
					this->_reader.popBufferFront();
				}
			}
		}
		if (this->_reader.empty())
		{
			this->_token._position = this->_reader.streamPosition();
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
			const ScanChar & c = this->_reader.front();
			this->_token._position = c._position;
			this->_token._text.push_back(c._value);
			throw DbcFormatException("Unknown token");
		}
	}

	const DbcToken & DbcScanner::token() const
	{
		return this->_token;
	}

}
