
#include <OptiScan/Parser/A2l/A2lKeyword.h>
#include <OptiScan/Parser/A2l/A2lTokenReader.h>
#include <OptiScan/Parser/FileException.h>
#include <OptiScan/Parser/TokenReaderUtils.h>
#include <vector>

using namespace std;

namespace OptiScan::Parser::A2l
{
	const size_t A2lTokenReader::UnknownBlockStackReserve = 16;

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
			result = !this->_tokenStack.empty();
		}
		else
		{
			result = 0 < this->_tokenStackCount && this->_tokenStackCount <= this->tokenStackSize();
		}
		return result;
	}

	void A2lTokenReader::matchKeyword(const string & id) const
	{
		if (!this->tryMatchKeyword(id))
		{
			string found;
			if (this->hasToken())
			{
				found = A2lTokenReader::tokenKindToText(this->token()._kind);
				if (this->token()._kind == A2lTokenKind::Identifier)
				{
					found += " '" + this->token()._text + "'";
				}
			}
			else
			{
				found = "no token";
			}
			this->throwFormatError("A2lTokenReader::matchKeyword",
				"Expected keyword '" + id + "', found " + found);
		}
	}

	void A2lTokenReader::matchToken(A2lTokenKind kind) const
	{
		if (!this->tryMatchToken(kind))
		{
			string const found = this->hasToken()
				? A2lTokenReader::tokenKindToText(this->token()._kind)
				: string("no token");
			this->throwFormatError("A2lTokenReader::matchToken",
				"Expected " + A2lTokenReader::tokenKindToText(kind) + ", found " + found);
		}
	}

	void A2lTokenReader::trackOneTimeKeyword(unordered_set<string> & oneTimeKeywords, const string & keyword)
	{
		if (oneTimeKeywords.find(keyword) != oneTimeKeywords.end())
		{
			throw FormatException("A2lTokenReader: Keyword '" + keyword + "' already encountered");
		}
		oneTimeKeywords.insert(keyword);
	}

	void A2lTokenReader::parseBlockBegin(const string & keyword)
	{
		this->matchToken(A2lTokenKind::EscapeBegin);
		this->readNextToken();
		this->matchKeyword(keyword);
		this->readNextToken();
	}

	void A2lTokenReader::parseBlockBeginAny(string & keyword)
	{
		this->matchToken(A2lTokenKind::EscapeBegin);
		this->readNextToken();
		this->matchToken(A2lTokenKind::Identifier);
		keyword = this->token()._text;
		this->readNextToken();
	}

	void A2lTokenReader::parseBlockEnd(const string & keyword)
	{
		this->matchToken(A2lTokenKind::EscapeEnd);
		this->readNextToken();
		this->matchKeyword(keyword);
		this->readNextToken();
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
			this->throwFormatError("A2lTokenReader::parseFloat64", "Float literal or integer literal expected");
		}
	}

	void A2lTokenReader::parseInt16(int16_t & value)
	{
		int64_t tmp;
		this->parseInt64(tmp);
		if (tmp < INT16_MIN || INT16_MAX < tmp)
		{
			this->throwFormatError("A2lTokenReader::parseInt16", "Int16 out of range");
		}
		value = static_cast<int16_t>(tmp);
	}

	void A2lTokenReader::parseInt32(int32_t & value)
	{
		int64_t tmp;
		this->parseInt64(tmp);
		if (tmp < INT32_MIN || INT32_MAX < tmp)
		{
			this->throwFormatError("A2lTokenReader::parseInt32", "Int32 out of range");
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
				this->throwFormatError("A2lTokenReader::parseInt64", "Hex literal is not int64");
			}
			value = static_cast<int64_t>(tmp);
		}
		else if (this->tryMatchToken(A2lTokenKind::IntegerLiteral))
		{
			value = TokenReaderUtils::literalIntegerTokenTextToInt64(this->token()._text);
		}
		else
		{
			this->throwFormatError("A2lTokenReader::parseInt64", "Hex literal or integer literal expected");
		}
		this->readNextToken();
	}

	void A2lTokenReader::parseString(string & value)
	{
		this->matchToken(A2lTokenKind::StringLiteral);
		// todo: process escape sequences
		value = TokenReaderUtils::literalStringTokenTextWithNestedStringToString(this->token()._text);
		this->readNextToken();
	}

	void A2lTokenReader::parseUInt8(uint8_t & value)
	{
		uint64_t tmp = 0;
		this->parseUInt64(tmp);
		if (UINT8_MAX < tmp)
		{
			this->throwFormatError("A2lTokenReader::parseUInt8", "Literal is not uint8");
		}
		value = static_cast<uint8_t>(tmp);
	}

	void A2lTokenReader::parseUInt16(uint16_t & value)
	{
		uint64_t tmp = 0;
		this->parseUInt64(tmp);
		if (UINT16_MAX < tmp)
		{
			this->throwFormatError("A2lTokenReader::parseUInt16", "Literal is not uint16");
		}
		value = static_cast<uint16_t>(tmp);
	}

	void A2lTokenReader::parseUInt32(uint32_t & value)
	{
		uint64_t tmp = 0;
		this->parseUInt64(tmp);
		if (UINT32_MAX < tmp)
		{
			this->throwFormatError("A2lTokenReader::parseUInt32", "Literal is not uint32");
		}
		value = static_cast<uint32_t>(tmp);
	}

	void A2lTokenReader::parseUInt64(uint64_t & value)
	{
		if (this->tryMatchToken(A2lTokenKind::HexLiteral))
		{
			value = TokenReaderUtils::literalHexIntegerTokenTextToUInt64(this->token()._text);
		}
		else if (this->tryMatchToken(A2lTokenKind::IntegerLiteral))
		{
			value = TokenReaderUtils::literalIntegerTokenTextToUInt64(this->token()._text);
		}
		else
		{
			this->throwFormatError("A2lTokenReader::parseUInt64", "Hex literal or integer literal expected");
		}
		this->readNextToken();
	}

	void A2lTokenReader::parseUnknownBlock()
	{
		string keyword;
		this->parseBlockBeginAny(keyword);
		this->parseUnknownBlockContent();
		this->parseBlockEnd(keyword);
	}

	void A2lTokenReader::parseUnknownBlockContent()
	{
		vector<A2lTokenKind> blockStack;
		blockStack.reserve(A2lTokenReader::UnknownBlockStackReserve);
		while (!blockStack.empty() || !this->tryMatchToken(A2lTokenKind::EscapeEnd))
		{
			bool isMatchRequired = false;
			switch (this->token()._kind)
			{
			case A2lTokenKind::EscapeBegin:
				blockStack.push_back(A2lTokenKind::EscapeEnd);
				break;
			case A2lTokenKind::LeftCurlyBracket:
				blockStack.push_back(A2lTokenKind::RightCurlyBracket);
				break;
			case A2lTokenKind::LeftParenthesis:
				blockStack.push_back(A2lTokenKind::RightParenthesis);
				break;
			case A2lTokenKind::LeftSquareBracket:
				blockStack.push_back(A2lTokenKind::RightSquareBracket);
				break;
			case A2lTokenKind::EscapeEnd:
			case A2lTokenKind::RightCurlyBracket:
			case A2lTokenKind::RightParenthesis:
			case A2lTokenKind::RightSquareBracket:
				isMatchRequired = true;
				break;
			case A2lTokenKind::None:
				this->throwFormatError("A2lTokenReader::parseUnknownBlockContent",
					"Unexpected end of file inside a block");
			default:
				break;
			}
			if (isMatchRequired)
			{
				if (blockStack.empty())
				{
					this->throwFormatError("A2lTokenReader::parseUnknownBlockContent",
						"Missing block open token for " + A2lTokenReader::tokenKindToText(this->token()._kind));
				}
				else if (this->token()._kind != blockStack.back())
				{
					this->throwFormatError("A2lTokenReader::parseUnknownBlockContent",
						"Expected " + A2lTokenReader::tokenKindToText(blockStack.back())
						+ ", found " + A2lTokenReader::tokenKindToText(this->token()._kind));
				}
				else
				{
					blockStack.pop_back();
				}
			}
			this->readNextToken();
		}
	}

	void A2lTokenReader::readNextToken()
	{
		if (this->_tokenStackCount == -1)
		{
			if (!this->_tokenStack.empty())
			{
				this->_tokenStack.pop_front();
			}
			if (this->_tokenStack.empty())
			{
				this->_scanner.scanNext();
				this->_tokenStack.push_back(this->_scanner.token());
			}
		}
		else
		{
			if (this->tokenStackSize() <= this->_tokenStackCount)
			{
				this->_scanner.scanNext();
				this->_tokenStack.push_back(this->_scanner.token());
			}
			this->_tokenStackCount++;
		}
	}

	const A2lScanner & A2lTokenReader::scanner() const
	{
		return this->_scanner;
	}

	void A2lTokenReader::throwFormatError(const string & context, const string & message) const
	{
		string position;
		if (this->hasToken())
		{
			const ScanPosition & tokenPosition = this->token()._position;
			position = " (line " + to_string(tokenPosition._line)
				+ ", column " + to_string(tokenPosition._charInLine) + ")";
		}
		throw FormatException(context + position + ": " + message);
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

	string A2lTokenReader::tokenKindToText(A2lTokenKind kind)
	{
		string result;
		switch (kind)
		{
		case A2lTokenKind::None:
			result = "end of file";
			break;
		case A2lTokenKind::Asterisk:
			result = "'*'";
			break;
		case A2lTokenKind::Comma:
			result = "','";
			break;
		case A2lTokenKind::CompatMinus:
			result = "'-'";
			break;
		case A2lTokenKind::Dot:
			result = "'.'";
			break;
		case A2lTokenKind::EqualSign:
			result = "'='";
			break;
		case A2lTokenKind::EscapeBegin:
			result = "'/begin'";
			break;
		case A2lTokenKind::EscapeEnd:
			result = "'/end'";
			break;
		case A2lTokenKind::FloatLiteral:
			result = "float literal";
			break;
		case A2lTokenKind::HexLiteral:
			result = "hex literal";
			break;
		case A2lTokenKind::Identifier:
			result = "identifier";
			break;
		case A2lTokenKind::IntegerLiteral:
			result = "integer literal";
			break;
		case A2lTokenKind::LeftCurlyBracket:
			result = "'{'";
			break;
		case A2lTokenKind::LeftParenthesis:
			result = "'('";
			break;
		case A2lTokenKind::LeftSquareBracket:
			result = "'['";
			break;
		case A2lTokenKind::RightCurlyBracket:
			result = "'}'";
			break;
		case A2lTokenKind::RightParenthesis:
			result = "')'";
			break;
		case A2lTokenKind::RightSquareBracket:
			result = "']'";
			break;
		case A2lTokenKind::Semicolon:
			result = "';'";
			break;
		case A2lTokenKind::StringLiteral:
			result = "string literal";
			break;
		default:
			result = "unknown token";
			break;
		}
		return result;
	}

	const deque<A2lToken> & A2lTokenReader::tokenStack() const
	{
		return this->_tokenStack;
	}

	void A2lTokenReader::tokenStackBegin()
	{
		if (this->_tokenStackCount != -1)
		{
			throw InvalidOperationException("Token stack begin error: token stack already in use");
		}
		if (this->_tokenStack.empty())
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
			this->_tokenStack.pop_front();
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

	int64_t A2lTokenReader::tokenStackSize() const
	{
		return static_cast<int64_t>(this->_tokenStack.size());
	}

	bool A2lTokenReader::tryMatchKeyword(const string & id) const
	{
		bool result = false;
		if (this->tryMatchToken(A2lTokenKind::Identifier))
		{
			result = this->token()._text == id;
		}
		return result;
	}

	bool A2lTokenReader::tryMatchToken(A2lTokenKind kind) const
	{
		exception_ptr error;
		return this->tryMatchToken(kind, error);
	}

	bool A2lTokenReader::tryMatchToken(A2lTokenKind kind, exception_ptr & error) const
	{
		error = nullptr;
		bool result = this->hasToken();
		if (!result)
		{
			error = make_exception_ptr(InvalidOperationException("A2lTokenReader: No token"));
		}
		else
		{
			result = this->token()._kind == kind;
			if (!result)
			{
				error = make_exception_ptr(FormatException("A2lTokenReader: Token mismatch"));
			}
		}

		return result;
	}

	bool A2lTokenReader::tryParseUInt16(uint16_t & value, exception_ptr & error)
	{
		error = nullptr;
		bool result = false;
		this->tokenStackBegin();
		try
		{
			// check if A2lTokenKind::HexLiteral or A2lTokenKind::IntegerLiteral in parseUInt64 method
			this->parseUInt16(value);
			this->tokenStackCommit();
			result = true;
		}
		catch (const FormatException &)
		{
			error = current_exception();
			this->tokenStackRollback();
		}
		return result;
	}

	bool A2lTokenReader::tryTokenStackBlockBegin(const string & keyword)
	{
		exception_ptr error;
		return this->tryTokenStackBlockBegin(keyword, error);
	}

	bool A2lTokenReader::tryTokenStackBlockBegin(const string & keyword, exception_ptr & error)
	{
		error = nullptr;
		string tmp;
		bool result = this->tryTokenStackBlockBeginAny(tmp, error);
		if (result)
		{
			result = keyword == tmp;
			if (!result)
			{
				error = make_exception_ptr(FormatException("A2lTokenReader: Keyword mismatch"));
			}
		}
		return result;
	}

	bool A2lTokenReader::tryTokenStackBlockBeginAny(string & keyword)
	{
		exception_ptr error;
		return this->tryTokenStackBlockBeginAny(keyword, error);
	}

	bool A2lTokenReader::tryTokenStackBlockBeginAny(string & keyword, exception_ptr & error)
	{
		error = nullptr;
		bool result = false;
		this->tokenStackBegin();
		try
		{
			if (this->tryMatchToken(A2lTokenKind::EscapeBegin, error))
			{
				this->readNextToken();
				if (this->tryMatchToken(A2lTokenKind::Identifier, error))
				{
					keyword = this->token()._text;
					result = true;
				}
			}
		}
		catch (const exception &)
		{
			error = current_exception();
		}
		this->tokenStackRollback();
		return result;
	}
}
