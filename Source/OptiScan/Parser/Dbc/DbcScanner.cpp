
#include <OptiScan/Parser/Dbc/DbcScanner.h>
#include <OptiScan/Parser/Dbc/DbcFormat.h>
#include <OptiScan/Parser/FileException.h>
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
				this->_token._text.push_back(this->_reader.popBufferFront()._value);
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
									this->_token._text.push_back(this->_reader.popBufferFront()._value);
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
								this->_token._text.push_back(this->_reader.popBufferFront()._value);
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
									this->_token._text.push_back(this->_reader.popBufferFront()._value);
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
									this->_token._text.push_back(this->_reader.popBufferFront()._value);
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
						throw FormatException("Missing decimal digit");
					}
					if (hasExponent && !hasExponentDigit)
					{
						throw FormatException("Missing exponent digit");
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
				this->_token._text.push_back(this->_reader.popBufferFront()._value);
				bool checkNext = true;
				while (checkNext)
				{
					if (!this->_reader.fillScanBuffer())
					{
						throw FormatException("Missing quotation mark");
					}
					const ScanChar & c = this->_reader.front();
					if (c._value == DbcFormat::StringEscapeStart)
					{
						this->_token._text.push_back(this->_reader.popBufferFront()._value);
						if (!this->_reader.fillScanBuffer())
						{
							throw FormatException("Missing escape marker");
						}
						const ScanChar & escaped = this->_reader.front();
						bool findMarker = true;
						for (size_t i = 0; findMarker && i < DbcFormat::StringEscapeItems.size(); i++)
						{
							findMarker = DbcFormat::StringEscapeItems[i]._escapeMarker != escaped._value;
						}
						if (findMarker)
						{
							throw FormatException("Invalid escape marker");
						}
					}
					else
					{
						checkNext = c._value != '"';
					}
					this->_token._text.push_back(this->_reader.popBufferFront()._value);
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
				this->_token._text.push_back(this->_reader.popBufferFront()._value);
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
		else if (this->_reader.readIdentifier(this->_token._position, this->_token._text))
		{
			this->_token._kind = DbcTokenKind::Identifier;
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
			throw FormatException("Unknown token");
		}
	}

	const DbcToken & DbcScanner::token() const
	{
		return this->_token;
	}

}
