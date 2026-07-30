
#include <OptiScan/Parser/Dbc/DbcScanner.h>

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
			throw std::invalid_argument("Unknown token");
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
