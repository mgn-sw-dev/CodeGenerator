
#include <OptiScan/Parser/Ldf/LdfScanner.h>
#include <OptiScan/Parser/FileException.h>

using namespace std;

namespace OptiScan::Parser::Ldf
{

	string const LdfScanner::StringCommentEnd("*/");
	string const LdfScanner::StringCommentStart("/*");
	string const LdfScanner::StringLineCommentStart("//");


	LdfScanner::LdfScanner(std::istream * input)
		: _scanBuffer()
		, _stream(input)
		, _streamPosition()
		, _token()
	{
	}

	bool LdfScanner::fillScanBuffer(size_t count)
	{
		bool result = true;
		for (size_t i = this->_scanBuffer.size(); result && i < count; i++)
		{
			result = this->readCharFromStreamToScanBuffer();
		}
		return result;
	}

	LdfScanChar LdfScanner::popScanBufferFront()
	{
		const LdfScanChar c = this->_scanBuffer.front();
		this->_scanBuffer.erase(this->_scanBuffer.begin());
		return c;
	}

	void LdfScanner::popScanBufferFrontToToken()
	{
		this->_token._text.push_back(this->_scanBuffer.front()._value);
		this->_scanBuffer.erase(this->_scanBuffer.begin());
	}

	bool LdfScanner::isCharDigit(const char & c)
	{
		return '0' <= c && c <= '9';
	}

	bool LdfScanner::isCharHexDigit(const char & c)
	{
		return LdfScanner::isCharDigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
	}

	bool LdfScanner::isCharIdentifier(const char & c)
	{
		return LdfScanner::isCharIdentifierStart(c) || LdfScanner::isCharDigit(c);
	}

	bool LdfScanner::isCharIdentifierStart(const char & c)
	{
		return false
			|| ('a' <= c && c <= 'z')
			|| ('A' <= c && c <= 'Z')
			|| c == '_'
		;
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

	bool LdfScanner::readCharFromStream(char & c)
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

	bool LdfScanner::readCharFromStreamToScanBuffer()
	{
		LdfScanChar c;
		c._position = this->_streamPosition;
		bool result = this->readCharFromStream(c._value);
		if (result)
		{
			this->_scanBuffer.push_back(c);
		}
		return result;
	}

	void LdfScanner::readComment()
	{
		size_t const startCount = LdfScanner::StringCommentStart.size();
		bool isValid = this->fillScanBuffer(startCount);
		if (isValid)
		{
			isValid = this->scanBufferStartsWith(LdfScanner::StringCommentStart);
		}
		if (!isValid)
		{
			throw FormatException("Invalid comment");
		}
		for (size_t i = 0; i < startCount; i++)
		{
			this->_scanBuffer.erase(this->_scanBuffer.begin());
		}
		size_t const endCount = LdfScanner::StringCommentEnd.size();
		bool checkNext = true;
		while (checkNext)
		{
			if (this->fillScanBuffer(startCount))
			{
				if (this->scanBufferStartsWith(LdfScanner::StringCommentStart))
				{
					throw FormatException("Nesting comment");
				}
			}
			if (!this->fillScanBuffer(endCount))
			{
				throw FormatException("Missing comment end");
			}
			if (!this->scanBufferStartsWith(LdfScanner::StringCommentEnd))
			{
				this->_scanBuffer.erase(this->_scanBuffer.begin());
			}
			else
			{
				checkNext = false;
				for (size_t i = 0; i < endCount; i++)
				{
					this->_scanBuffer.erase(this->_scanBuffer.begin());
				}
			}
		}
	}

	void LdfScanner::readLineComment()
	{
		bool isValid = this->fillScanBuffer(LdfScanner::StringLineCommentStart.size());
		if (isValid)
		{
			isValid = this->scanBufferStartsWith(LdfScanner::StringLineCommentStart);
		}
		if (!isValid)
		{
			throw FormatException("Invalid line comment");
		}
		for (size_t i = 0; i < LdfScanner::StringLineCommentStart.size(); i++)
		{
			this->_scanBuffer.erase(this->_scanBuffer.begin());
		}
		bool checkNext = true;
		while (checkNext)
		{
			checkNext = this->fillScanBuffer();
			if (checkNext)
			{
				checkNext = !LdfScanner::isCharLineEnd(this->_scanBuffer.front()._value);
				this->_scanBuffer.erase(this->_scanBuffer.begin());
			}
		}
	}

	bool LdfScanner::scanBufferStartsWith(const string & pattern) const
	{
		bool result = pattern.size() <= this->_scanBuffer.size();
		for (size_t i = 0; result && i < pattern.size(); i++)
		{
			result = pattern[i] == this->_scanBuffer[i]._value;
		}
		return result;
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
					if (this->fillScanBuffer())
					{
						checkNext = LdfScanner::isCharWhiteSpace(this->_scanBuffer.front()._value);
						if (checkNext)
						{
							this->_scanBuffer.erase(this->_scanBuffer.begin());
						}
					}
				}
				// Skip comment
				if (!checkNext)
				{
					if (this->fillScanBuffer(LdfScanner::StringCommentStart.size()))
					{
						checkNext = this->scanBufferStartsWith(LdfScanner::StringCommentStart);
						if (checkNext)
						{
							this->readComment();
						}
					}
				}
				// Skip line comment
				if (!checkNext)
				{
					if (this->fillScanBuffer(LdfScanner::StringLineCommentStart.size()))
					{
						checkNext = this->scanBufferStartsWith(LdfScanner::StringLineCommentStart);
						if (checkNext)
						{
							this->readLineComment();
						}
					}
				}
			}
		}
		if (this->_scanBuffer.empty())
		{
			this->_token._position = this->_streamPosition;
		}
		else if (this->tryReadIdentifier())
		{
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
			const LdfScanChar & c = this->_scanBuffer.front();
			this->_token._position = c._position;
			this->_token._text.push_back(c._value);
			throw FormatException("Unknown token");
		}
	}

	const LdfScanPosition & LdfScanner::streamPosition() const
	{
		return this->_streamPosition;
	}

	const LdfToken & LdfScanner::token() const
	{
		return this->_token;
	}

	bool LdfScanner::tryReadIdentifier()
	{
		bool result = false;
		if (this->fillScanBuffer())
		{
			const LdfScanChar & c = this->_scanBuffer.front();
			if (LdfScanner::isCharIdentifierStart(c._value))
			{
				result = true;
				this->_token._kind = LdfTokenKind::Identifier;
				this->_token._position = c._position;
				this->_token._text.push_back(c._value);
				this->_scanBuffer.erase(this->_scanBuffer.begin());
				bool checkNext = true;
				while (checkNext)
				{
					checkNext = this->fillScanBuffer();
					if (checkNext)
					{
						const LdfScanChar & c = this->_scanBuffer.front();
						checkNext = LdfScanner::isCharIdentifier(c._value);
						if (checkNext)
						{
							this->_token._text.push_back(c._value);
							this->_scanBuffer.erase(this->_scanBuffer.begin());
						}
					}
				}
			}
		}
		return result;
	}

	bool LdfScanner::tryReadLiteralHexInteger()
	{
		bool result = false;
		if (this->fillScanBuffer(3))
		{
			if (this->_scanBuffer[0]._value == '0'
				&& this->_scanBuffer[1]._value == 'x'
				&& LdfScanner::isCharHexDigit(this->_scanBuffer[2]._value))
			{
				result = true;
				this->_token._kind = LdfTokenKind::LiteralHexInteger;
				this->_token._position = this->_scanBuffer[0]._position;
				this->_token._text.push_back(this->_scanBuffer[0]._value);
				this->_scanBuffer.erase(this->_scanBuffer.begin());
				this->_token._text.push_back(this->_scanBuffer[0]._value);
				this->_scanBuffer.erase(this->_scanBuffer.begin());
				this->_token._text.push_back(this->_scanBuffer[0]._value);
				this->_scanBuffer.erase(this->_scanBuffer.begin());
				bool checkNext = true;
				while (checkNext)
				{
					checkNext = this->fillScanBuffer();
					if (checkNext)
					{
						const LdfScanChar & c = this->_scanBuffer.front();
						checkNext = LdfScanner::isCharHexDigit(c._value);
						if (checkNext)
						{
							this->_token._text.push_back(c._value);
							this->_scanBuffer.erase(this->_scanBuffer.begin());
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
		if (this->fillScanBuffer())
		{
			const LdfScanChar & c = this->_scanBuffer.front();
			bool hasDigit = false;
			if (c._value == '+' || c._value == '-')
			{
				result = true;
			}
			else if (LdfScanner::isCharDigit(c._value))
			{
				result = true;
				hasDigit = true;
			}
			if (result)
			{
				this->_token._kind = LdfTokenKind::LiteralInteger;
				this->_token._position = c._position;
				this->_token._text.push_back(c._value);
				this->_scanBuffer.erase(this->_scanBuffer.begin());
				bool checkNext = true;
				bool hasDecimalSeparator = false;
				bool hasDecimalDigit = false;
				while (checkNext)
				{
					if (!this->fillScanBuffer())
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
						const LdfScanChar & c = this->_scanBuffer.front();
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
						else if (LdfScanner::isCharDigit(c._value))
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
							this->_token._text.push_back(c._value);
							this->_scanBuffer.erase(this->_scanBuffer.begin());
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
		if (this->fillScanBuffer())
		{
			const LdfScanChar & c = this->_scanBuffer.front();
			if (c._value == '"')
			{
				result = true;
				this->_token._kind = LdfTokenKind::LiteralString;
				this->_token._position = c._position;
				this->_token._text.push_back(c._value);
				this->_scanBuffer.erase(this->_scanBuffer.begin());
				bool checkNext = true;
				while (checkNext)
				{
					if (!this->fillScanBuffer())
					{
						throw FormatException("Missing quotation mark");
					}
					const LdfScanChar & c = this->_scanBuffer.front();
					checkNext = c._value != '"';
					this->_token._text.push_back(c._value);
					this->_scanBuffer.erase(this->_scanBuffer.begin());
				}
			}
		}
		return result;
	}

	bool LdfScanner::tryReadOperator()
	{
		bool result = false;
		if (this->fillScanBuffer())
		{
			result = true;
			const LdfScanChar & c = this->_scanBuffer.front();
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
				this->_token._position = c._position;
				this->_token._text.push_back(c._value);
				this->_scanBuffer.erase(this->_scanBuffer.begin());
			}
		}
		return result;
	}


	LdfToken::LdfToken()
		: _kind(LdfTokenKind::None)
		, _position()
		, _text()
	{
	}

}
