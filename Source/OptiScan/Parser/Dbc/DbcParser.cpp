
#include <OptiScan/Parser/Dbc/DbcParser.h>

using namespace std;

namespace OptiScan::Parser::Dbc
{
	DbcParser::DbcParser(istream * input)
		: _scanner(input)
		, _tokenStack()
		, _tokenStackCount(-1)
	{
	}

	void DbcParser::parse(DbcDatabase & dbcDatabase)
	{
		dbcDatabase = DbcDatabase();

		this->readNextToken();
	}

	void DbcParser::readNextToken()
	{
		if (this->_tokenStackCount == -1)
		{
			if (0 < this->_tokenStack.size())
			{
				this->_tokenStack.erase(this->_tokenStack.begin());
			}
			if (0 == this->_tokenStack.size())
			{
				this->_scanner.scanNext();
				this->_tokenStack.push_back(this->_scanner.token());
			}
		}
		else
		{
			if (this->_tokenStack.size() <= this->_tokenStackCount)
			{
				this->_scanner.scanNext();
				this->_tokenStack.push_back(this->_scanner.token());
			}
			this->_tokenStackCount++;
		}
	}
}
