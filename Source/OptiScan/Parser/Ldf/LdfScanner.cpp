
#include <OptiScan/Parser/Ldf/LdfScanner.h>
#include <OptiScan/Parser/FileException.h>

using namespace std;

namespace OptiScan::Parser::Ldf
{

	string const LdfScanner::StringCommentEnd("*/");
	string const LdfScanner::StringCommentStart("/*");
	string const LdfScanner::StringLineCommentStart("//");


	LdfScanner::LdfScanner(std::istream * input)
		: _reader(input)
		, _token()
	{
	}
	
	bool LdfScanner::isCharHexDigit(const char & c)
	{
		return CharReader::isCharDigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
	}

	bool LdfScanner::isCharLineEnd(const char & c)
	{
		return c == static_cast<char>(0x0A);
	}

	bool LdfScanner::isCharWhiteSpace(const char & c)
	{
		return false
			|| c == static_cast<char>(0x09)
			|| c == '\r'
			|| c == static_cast<char>(0x20)
			|| LdfScanner::isCharLineEnd(c)
		;
	}

	void LdfScanner::readComment()
	{
		size_t const startCount = LdfScanner::StringCommentStart.size();
		bool isValid = this->_reader.fillScanBuffer(startCount);
		if (isValid)
		{
			isValid = this->_reader.bufferStartsWith(LdfScanner::StringCommentStart);
		}
		if (!isValid)
		{
			throw FormatException("Invalid comment");
		}
		for (size_t i = 0; i < startCount; i++)
		{
			this->_reader.popBufferFront();
		}
		size_t const endCount = LdfScanner::StringCommentEnd.size();
		bool checkNext = true;
		while (checkNext)
		{
			if (this->_reader.fillScanBuffer(startCount))
			{
				if (this->_reader.bufferStartsWith(LdfScanner::StringCommentStart))
				{
					throw FormatException("Nesting comment");
				}
			}
			if (!this->_reader.fillScanBuffer(endCount))
			{
				throw FormatException("Missing comment end");
			}
			if (!this->_reader.bufferStartsWith(LdfScanner::StringCommentEnd))
			{
				this->_reader.popBufferFront();
			}
			else
			{
				checkNext = false;
				for (size_t i = 0; i < endCount; i++)
				{
					this->_reader.popBufferFront();
				}
			}
		}
	}

	void LdfScanner::readLineComment()
	{
		bool isValid = this->_reader.fillScanBuffer(LdfScanner::StringLineCommentStart.size());
		if (isValid)
		{
			isValid = this->_reader.bufferStartsWith(LdfScanner::StringLineCommentStart);
		}
		if (!isValid)
		{
			throw FormatException("Invalid line comment");
		}
		for (size_t i = 0; i < LdfScanner::StringLineCommentStart.size(); i++)
		{
			this->_reader.popBufferFront();
		}
		bool checkNext = true;
		while (checkNext)
		{
			checkNext = this->_reader.fillScanBuffer();
			if (checkNext)
			{
				checkNext = !LdfScanner::isCharLineEnd(this->_reader.popBufferFront()._value);
			}
		}
	}

	void LdfScanner::scanNext()
	{
		this->_token = LdfToken();
		// Skip white space, comment, line comment
		{
			bool checkNext = true;
			while (checkNext)
			{
				checkNext = false;
				// Skip white space
				if (!checkNext)
				{
					if (this->_reader.fillScanBuffer())
					{
						checkNext = LdfScanner::isCharWhiteSpace(this->_reader.front()._value);
						if (checkNext)
						{
							this->_reader.popBufferFront();
						}
					}
				}
				// Skip comment
				if (!checkNext)
				{
					if (this->_reader.fillScanBuffer(LdfScanner::StringCommentStart.size()))
					{
						checkNext = this->_reader.bufferStartsWith(LdfScanner::StringCommentStart);
						if (checkNext)
						{
							this->readComment();
						}
					}
				}
				// Skip line comment
				if (!checkNext)
				{
					if (this->_reader.fillScanBuffer(LdfScanner::StringLineCommentStart.size()))
					{
						checkNext = this->_reader.bufferStartsWith(LdfScanner::StringLineCommentStart);
						if (checkNext)
						{
							this->readLineComment();
						}
					}
				}
			}
		}
		if (this->_reader.empty())
		{
			this->_token._position = this->_reader.streamPosition();
		}
		else if (this->_reader.readIdentifier(this->_token._position, this->_token._text))
		{
			this->_token._kind = LdfTokenKind::Identifier;
		}
		else if (this->tryReadLiteralHexInteger())
		{
		}
		else if (this->tryReadLiteralIntegerOrReal())
		{
		}
		else if (this->tryReadLiteralString())
		{
		}
		else if (this->tryReadOperator())
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

	const LdfToken & LdfScanner::token() const
	{
		return this->_token;
	}

	bool LdfScanner::tryReadLiteralHexInteger()
	{
		bool result = false;
		if (this->_reader.fillScanBuffer(3))
		{
			if (this->_reader.at(0)._value == '0'
				&& this->_reader.at(1)._value == 'x'
				&& LdfScanner::isCharHexDigit(this->_reader.at(2)._value))
			{
				result = true;
				this->_token._kind = LdfTokenKind::LiteralHexInteger;
				this->_token._position = this->_reader.at(0)._position;
				this->_token._text.push_back(this->_reader.popBufferFront()._value);
				this->_token._text.push_back(this->_reader.popBufferFront()._value);
				this->_token._text.push_back(this->_reader.popBufferFront()._value);
				bool checkNext = true;
				while (checkNext)
				{
					checkNext = this->_reader.fillScanBuffer();
					if (checkNext)
					{
						const ScanChar & c = this->_reader.front();
						checkNext = LdfScanner::isCharHexDigit(c._value);
						if (checkNext)
						{
							this->_token._text.push_back(this->_reader.popBufferFront()._value);
						}
					}
				}
			}
		}
		return result;
	}

	bool LdfScanner::tryReadLiteralIntegerOrReal()
	{
		bool result = false;
		if (this->_reader.fillScanBuffer())
		{
			const ScanChar & c = this->_reader.front();
			bool hasDigit = false;
			if (c._value == '+' || c._value == '-')
			{
				result = true;
			}
			else if (CharReader::isCharDigit(c._value))
			{
				result = true;
				hasDigit = true;
			}
			if (result)
			{
				this->_token._kind = LdfTokenKind::LiteralInteger;
				const ScanChar & tmp = this->_reader.popBufferFront();
				this->_token._position = tmp._position;
				this->_token._text.push_back(tmp._value);
				bool checkNext = true;
				bool hasDecimalSeparator = false;
				bool hasDecimalDigit = false;
				while (checkNext)
				{
					if (!this->_reader.fillScanBuffer())
					{
						if (!hasDigit)
						{
							throw FormatException("Missing digit");
						}
						else if (hasDecimalSeparator && !hasDecimalDigit)
						{
							throw FormatException("Missing decimal digit");
						}
						checkNext = false;
					}
					if (checkNext)
					{
						const ScanChar & c = this->_reader.front();
						if (c._value == '.')
						{
							if (!hasDigit)
							{
								throw FormatException("Missing digit");
							}
							else if (hasDecimalSeparator)
							{
								throw FormatException("Invalid decimal separator");
							}
							hasDecimalSeparator = true;
							this->_token._kind = LdfTokenKind::LiteralReal;
						}
						else if (CharReader::isCharDigit(c._value))
						{
							if (!hasDigit)
							{
								hasDigit = true;
							}
							else if (hasDecimalSeparator && !hasDecimalDigit)
							{
								hasDecimalDigit = true;
							}
						}
						else
						{
							if (!hasDigit)
							{
								throw FormatException("Missing digit");
							}
							else if (hasDecimalSeparator && !hasDecimalDigit)
							{
								throw FormatException("Missing decimal digit");
							}
							checkNext = false;
						}
						if (checkNext)
						{
							this->_token._text.push_back(this->_reader.popBufferFront()._value);
						}
					}
				}
			}
		}
		return result;
	}

	bool LdfScanner::tryReadLiteralString()
	{
		bool result = false;
		if (this->_reader.fillScanBuffer())
		{
			const ScanChar & c = this->_reader.front();
			if (c._value == '"')
			{
				result = true;
				this->_token._kind = LdfTokenKind::LiteralString;
				const ScanChar & tmp = this->_reader.popBufferFront();
				this->_token._position = tmp._position;
				this->_token._text.push_back(tmp._value);
				bool checkNext = true;
				while (checkNext)
				{
					if (!this->_reader.fillScanBuffer())
					{
						throw FormatException("Missing quotation mark");
					}
					const ScanChar & c = this->_reader.popBufferFront();
					checkNext = c._value != '"';
					this->_token._text.push_back(c._value);
				}
			}
		}
		return result;
	}

	bool LdfScanner::tryReadOperator()
	{
		bool result = false;
		if (this->_reader.fillScanBuffer())
		{
			result = true;
			const ScanChar & c = this->_reader.front();
			if (c._value == ':')
			{
				this->_token._kind = LdfTokenKind::OperatorColon;
			}
			else if (c._value == ',')
			{
				this->_token._kind = LdfTokenKind::OperatorComma;
			}
			else if (c._value == '=')
			{
				this->_token._kind = LdfTokenKind::OperatorEqualSign;
			}
			else if (c._value == '{')
			{
				this->_token._kind = LdfTokenKind::OperatorLeftCurlyBracket;
			}
			else if (c._value == '}')
			{
				this->_token._kind = LdfTokenKind::OperatorRightCurlyBracket;
			}
			else if (c._value == ';')
			{
				this->_token._kind = LdfTokenKind::OperatorSemicolon;
			}
			else
			{
				result = false;
			}
			if (result)
			{
				const ScanChar & c = this->_reader.popBufferFront();
				this->_token._position = c._position;
				this->_token._text.push_back(c._value);
			}
		}
		return result;
	}

}
