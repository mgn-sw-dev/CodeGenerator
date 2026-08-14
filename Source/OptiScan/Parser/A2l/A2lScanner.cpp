
#include <OptiScan/Parser/A2l/A2lScanner.h>

#include "OptiScan/Parser/FileException.h"

using namespace std;

namespace OptiScan::Parser::A2l
{
	char const A2lScanner::CharBackSlash('\\');
	char const A2lScanner::CharCarriageReturn(0x0D);
	char const A2lScanner::CharFormFeed(0x0C);
	char const A2lScanner::CharHorizontalTabulation(0x09);
	char const A2lScanner::CharLineFeed(0x0A);
	char const A2lScanner::CharQuotationMark('"');
	char const A2lScanner::CharSlash('/');
	char const A2lScanner::CharSpace(0x20);
	char const A2lScanner::CharVerticalTabulation(0x0B);
	string const A2lScanner::StringCommentEnd("*/");
	string const A2lScanner::StringCommentStart("/*");
	string const A2lScanner::StringEscapeBegin("/begin");
	string const A2lScanner::StringEscapeEnd("/end");
	string const A2lScanner::StringHexLowerStart("0x");
	string const A2lScanner::StringHexUpperStart("0X");
	string const A2lScanner::StringLineCommentStart("//");
	
	A2lScanner::A2lScanner(std::istream * input)
		: _compatStyles()
		, _reader(input)
		, _token()
	{
	}

	bool A2lScanner::checkNextIsSeparator(size_t count)
	{
		bool result = !this->_reader.fillScanBuffer(count + 1);
		if (!result)
		{
			char const c = this->_reader.at(count)._value;
			result = true
				&& !CharReader::isCharIdentifier(c)
				&& c != '+'
				&& c != '-'
				&& c != '"'
			;
		}
		return result;
	}

	void A2lScanner::commitToken(A2lTokenKind kind, size_t count)
	{
		this->_token._kind = kind;
		this->_token._position = this->_reader.at(0)._position;
		this->_token._text = this->_reader.popStringFromBufferFront(count);
	}

	void A2lScanner::getCompatStyles(A2lScannerCompatStyles & compatStyles) const
	{
		compatStyles = this->_compatStyles;
	}
	
	bool A2lScanner::isCharLineEnd(const char & c)
	{
		return c == A2lScanner::CharLineFeed;
	}
	
	bool A2lScanner::isCharWhiteSpace(const char & c)
	{
		return false
			|| c == A2lScanner::CharCarriageReturn
			|| c == A2lScanner::CharFormFeed
			|| c == A2lScanner::CharHorizontalTabulation
			|| c == A2lScanner::CharLineFeed
			|| c == A2lScanner::CharSpace
			|| c == A2lScanner::CharVerticalTabulation
		;
	}

	void A2lScanner::readComment()
	{
		size_t const startCount = A2lScanner::StringCommentStart.size();
		size_t const endCount = A2lScanner::StringCommentEnd.size();
		size_t count = startCount;
		bool isValid = this->_reader.tryMatchBufferStart(A2lScanner::StringCommentStart);
		bool checkNext = true;
		while (isValid && checkNext)
		{
			isValid = this->_reader.fillScanBuffer(count + 1);
			if (isValid)
			{
				count++;
				// check not nesting
				if (2 * startCount <= count)
				{
					isValid = !this->_reader.bufferStartsWith(A2lScanner::StringCommentStart, count - startCount);
					if (!isValid)
					{
						throw FormatException("Al2Scanner: Nesting comment");
					}
				}
				// check for end
				if (startCount + endCount <= count)
				{
					checkNext = !this->_reader.bufferStartsWith(A2lScanner::StringCommentEnd, count - endCount);
				}
			}
		}
		if (isValid)
		{
			this->_reader.popStringFromBufferFront(count);
		}
		else
		{
			throw FormatException("A2lScanner: Invalid comment");
		}
	}

	void A2lScanner::readLineComment()
	{
		size_t count = A2lScanner::StringLineCommentStart.size();
		bool isValid = this->_reader.tryMatchBufferStart(A2lScanner::StringLineCommentStart);
		if (isValid)
		{
			bool checkNext = true;
			while (checkNext)
			{
				checkNext = this->_reader.fillScanBuffer(count + 1);
				if (checkNext)
				{
					checkNext = !A2lScanner::isCharLineEnd(this->_reader.at(count)._value);
					if (checkNext)
					{
						count++;
					}
				}
			}
		}
		if (isValid)
		{
			this->_reader.popStringFromBufferFront(count);
		}
		else
		{
			throw FormatException("A2lScanner: Invalid line comment");
		}
	}

	void A2lScanner::scanNext()
	{
		this->_token = A2lToken();
		// Skip white space, comment, line comment
		bool checkNext = true;
		while (checkNext)
		{
			checkNext = false;
			// Skip white space
			if (!checkNext)
			{
				if (this->_reader.fillScanBuffer())
				{
					checkNext = A2lScanner::isCharWhiteSpace(this->_reader.front()._value);
					if (checkNext)
					{
						this->_reader.popScanCharFromBufferFront();
					}
				}
			}
			// skip comment
			if (!checkNext)
			{
				if (this->_reader.tryMatchBufferStart(A2lScanner::StringCommentStart))
				{
					this->readComment();
					checkNext = true;
				}
			}
			// Skip line comment
			if (!checkNext)
			{
				if (this->_reader.tryMatchBufferStart(A2lScanner::StringLineCommentStart))
				{
					this->readLineComment();
					checkNext = true;
				}
			}
		}
		if (this->_reader.empty())
		{
			this->_token._position = this->_reader.streamPosition();
		}
		else if (this->tryReadEscape())
		{
		}
		else if (this->tryReadFloatOrIntegerLiteral())
		{
		}
		else if (this->tryReadHexLiteral())
		{
		}
		else if (this->tryReadIdentifier())
		{
		}
		else if (this->tryReadOperator())
		{
		}
		else if (this->tryReadStringLiteral())
		{
		}
		else
		{
			const ScanChar & c = this->_reader.front();
			this->_token._position = c._position;
			this->_token._text.push_back(c._value);
			throw FormatException("A2lScanner: Unknown token: " + this->_token._text);
		}
	}

	void A2lScanner::setCompatStyles(const A2lScannerCompatStyles & compatStyles)
	{
		this->_compatStyles = compatStyles;
	}

	const A2lToken & A2lScanner::token() const
	{
		return this->_token;
	}

	bool A2lScanner::tryReadEscape()
	{
		A2lTokenKind kind = A2lTokenKind::None;
		size_t count = A2lScanner::StringEscapeBegin.size();
		if (this->_reader.tryMatchBufferStart(A2lScanner::StringEscapeBegin))
		{
			kind = A2lTokenKind::EscapeBegin;
		}
		if (kind == A2lTokenKind::None)
		{
			count = A2lScanner::StringEscapeEnd.size();
			if (this->_reader.tryMatchBufferStart(A2lScanner::StringEscapeEnd))
			{
				kind = A2lTokenKind::EscapeEnd;
			}
		}
		bool result = kind != A2lTokenKind::None;
		if (result)
		{
			result = this->checkNextIsSeparator(count);
		}
		if (result)
		{
			this->commitToken(kind, count);
		}
		return result;
	}

	bool A2lScanner::tryReadFloatOrIntegerLiteral()
	{
		bool hasDecimalDigit = false;
		bool hasDecimalSeparator = false;
		bool hasDigit = false;
		bool hasExponent = false;
		bool hasExponentDigit = false;
		bool hasExponentSign = false;
		bool hasSign = false;
		size_t count = 0;
		bool checkNext = true;
		while (checkNext)
		{
			checkNext = this->_reader.fillScanBuffer(count + 1);
			if (checkNext)
			{
				char const c = this->_reader.at(count)._value;
				if (CharReader::isCharDigit(c))
				{
					if (hasExponent)
					{
						hasExponentDigit = true;
					}
					else if (hasDecimalSeparator)
					{
						hasDecimalDigit = true;
					}
					else
					{
						hasDigit = true;
					}
				}
				else if (CharReader::isCharDecimalSeparator(c))
				{
					if (hasDecimalSeparator || hasExponent)
					{
						checkNext = false;
					}
					else
					{
						hasDecimalSeparator = true;
					}
				}
				else if (CharReader::isCharExponentStart(c))
				{
					if ((!hasDecimalSeparator && !hasDigit)
						|| (hasDecimalSeparator && !hasDecimalDigit)
						|| hasExponent)
					{
						checkNext = false;
					}
					else
					{
						hasExponent = true;
					}
				}
				else if (c == '+' || c == '-')
				{
					if (hasExponent)
					{
						if (hasExponentDigit || hasExponentSign)
						{
							checkNext = false;
						}
						else
						{
							hasExponentSign = true;
						}
					}
					else
					{
						if (hasSign || hasDigit || hasDecimalSeparator || hasDecimalDigit)
						{
							checkNext = false;
						}
						else
						{
							hasSign = true;
						}
					}
				}
				else
				{
					checkNext = false;
				}
			}
			if (checkNext)
			{
				count++;
			}
		}
		bool result = true
			&& (!hasDecimalSeparator || hasDecimalDigit)
			&& (!hasExponent || hasExponentDigit)
			&& (hasDigit || hasDecimalDigit)
		;
		if (result)
		{
			result = this->checkNextIsSeparator(count);
		}
		if (result)
		{
			if (hasDecimalSeparator || hasExponent)
			{
				this->commitToken(A2lTokenKind::FloatLiteral, count);
			}
			else
			{
				this->commitToken(A2lTokenKind::IntegerLiteral, count);
			}
		}
		return result;
	}

	bool A2lScanner::tryReadHexLiteral()
	{
		size_t count = A2lScanner::StringHexLowerStart.size();
		bool result = this->_reader.tryMatchBufferStart(A2lScanner::StringHexLowerStart);
		if (!result)
		{
			count = A2lScanner::StringHexUpperStart.size();
			result = this->_reader.tryMatchBufferStart(A2lScanner::StringHexUpperStart);
		}
		if (result)
		{
			result = this->_reader.fillScanBuffer(count + 1);
			if (result)
			{
				result = CharReader::isCharHexDigit(this->_reader.at(count)._value);
				if (result)
				{
					count++;
					bool checkNext = true;
					while (checkNext)
					{
						checkNext = this->_reader.fillScanBuffer(count + 1);
						if (checkNext)
						{
							checkNext = CharReader::isCharHexDigit(this->_reader.at(count)._value);
							if (checkNext)
							{
								count++;
							}
						}
					}
				}
			}
		}
		if (result)
		{
			result = this->checkNextIsSeparator(count);
		}
		if (result)
		{
			this->commitToken(A2lTokenKind::HexLiteral, count);
		}
		return result;
	}

	bool A2lScanner::tryReadIdentifier()
	{
		size_t count = 0;
		bool result = this->_reader.fillScanBuffer(count + 1);
		if (result)
		{
			result = CharReader::isCharIdentifierStart(this->_reader.at(count)._value);
			if (result)
			{
				count++;
				bool checkNext = true;
				while (checkNext)
				{
					checkNext = this->_reader.fillScanBuffer(count + 1);
					if (checkNext)
					{
						checkNext = CharReader::isCharIdentifier(this->_reader.at(count)._value);
						if (checkNext)
						{
							count++;
						}
					}
				}
			}
		}
		if (result)
		{
			result = this->checkNextIsSeparator(count);
		}
		if (result)
		{
			this->commitToken(A2lTokenKind::Identifier, count);
		}
		return result;
	}

	bool A2lScanner::tryReadOperator()
	{
		bool result = this->_reader.fillScanBuffer();
		if (result)
		{
			A2lTokenKind kind = A2lTokenKind::None;
			char const c = this->_reader.front()._value;
			switch (c)
			{
			case '*':
				kind = A2lTokenKind::Asterisk;
				break;
			case ',':
				kind = A2lTokenKind::Comma;
				break;
			case '.':
				kind = A2lTokenKind::Dot;
				break;
			case '=':
				kind = A2lTokenKind::EqualSign;
				break;
			case '{':
				kind = A2lTokenKind::LeftCurlyBracket;
				break;
			case '(':
				kind = A2lTokenKind::LeftParenthesis;
				break;
			case '[':
				kind = A2lTokenKind::LeftSquareBracket;
				break;
			case '}':
				kind = A2lTokenKind::RightCurlyBracket;
				break;
			case ']':
				kind = A2lTokenKind::RightSquareBracket;
				break;
			case ')':
				kind = A2lTokenKind::RightParenthesis;
				break;
			case ';':
				kind = A2lTokenKind::Semicolon;
				break;
			case '-':
				if (this->_compatStyles._allowCompatMinus)
				{
					kind = A2lTokenKind::CompatMinus;
				}
				break;
			default:
				result = false;
				break;
			}
			if (result)
			{
				this->commitToken(kind, 1);
			}
		}
		return result;
	}

	bool A2lScanner::tryReadStringLiteral()
	{
		size_t count = 1;
		bool result = this->_reader.tryMatchBufferChar(A2lScanner::CharQuotationMark);
		bool checkNext = true;
		bool hasBackSlashEscape = false;
		bool hasQuotationMark = false;
		while (result && checkNext)
		{
			if (!this->_reader.fillScanBuffer(count + 1))
			{
				result = hasQuotationMark;
				checkNext = false;
				if (!result)
				{
					throw FormatException("A2lScanner: Missing quotation mark at the end");
				}
			}
			else
			{
				char const c = this->_reader.at(count)._value;
				if (hasQuotationMark && c != '"')
				{
					checkNext = false;
				}
				else if (hasQuotationMark && c == '"')
				{
					count++;
					hasQuotationMark = false;
				}
				else if (hasBackSlashEscape)
				{
					result = false
						|| c == '\''
						|| c == '"'
						|| c == '\\'
						|| c == 'n'
						|| c == 'r'
						|| c == 't'
					;
					hasBackSlashEscape = false;
					if (result)
					{
						count++;
					}
					else
					{
						throw FormatException("A2lScanner: Invalid escape sequence");
					}
				}
				else if (c == '\\')
				{
					count++;
					hasBackSlashEscape = true;
				}
				else if (c == '"')
				{
					count++;
					hasQuotationMark = true;
				}
				else if (A2lScanner::isCharLineEnd(c))
				{
					if (this->_compatStyles._allowLineEndInStringLiteral)
					{
						count++;
					}
					else
					{
						throw FormatException("A2lScanner: Invalid line end; Activate compatibility mode AllowLineEndInStringLiteral.");
					}
				}
				else
				{
					count++;
				}
			}
		}
		if (result)
		{
			result = this->checkNextIsSeparator(count);
		}
		if (result)
		{
			this->commitToken(A2lTokenKind::StringLiteral, count);
		}
		return result;
	}
}
