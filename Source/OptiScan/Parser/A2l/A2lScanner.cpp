
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
		bool isValid = this->_reader.fillScanBuffer(count);
		if (isValid)
		{
			isValid = this->_reader.bufferStartsWith(A2lScanner::StringCommentStart);
		}
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
			for (size_t i = 0; i < count; i++)
			{
				this->_reader.popBufferFront();
			}
		}
		else
		{
			throw FormatException("A2lScanner: Invalid comment");
		}
	}

	void A2lScanner::readLineComment()
	{
		size_t count = A2lScanner::StringCommentStart.size();
		bool isValid = this->_reader.fillScanBuffer(count);
		if (isValid)
		{
			isValid = this->_reader.bufferStartsWith(A2lScanner::StringLineCommentStart);
		}
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
			for (size_t i = 0; i < count; i++)
			{
				this->_reader.popBufferFront();
			}
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
						this->_reader.popBufferFront();
					}
				}
			}
			// skip comment
			if (!checkNext)
			{
				if (this->_reader.fillScanBuffer(A2lScanner::StringCommentStart.size()))
				{
					checkNext = this->_reader.bufferStartsWith(A2lScanner::StringCommentStart);
					if (checkNext)
					{
						this->readComment();
					}
				}
			}
			// Skip line comment
			if (!checkNext)
			{
				if (this->_reader.fillScanBuffer(A2lScanner::StringLineCommentStart.size()))
				{
					checkNext = this->_reader.bufferStartsWith(A2lScanner::StringLineCommentStart);
					if (checkNext)
					{
						this->readLineComment();
					}
				}
			}
		}
		if (this->_reader.empty())
		{
			this->_token._position = this->_reader.streamPosition();
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
}
