
#include <OptiScan/Parser/A2l/A2lKeyword.h>
#include <OptiScan/Parser/A2l/A2lTokenReader.h>
#include <OptiScan/Parser/FileException.h>
#include <OptiScan/Parser/TokenReaderUtils.h>

using namespace std;

namespace OptiScan::Parser::A2l
{
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
			result = 0 < this->_tokenStack.size();
		}
		else
		{
			result = 0 < this->_tokenStackCount && this->_tokenStackCount <= this->_tokenStack.size();
		}
		return result;
	}

	const A2lScanner & A2lTokenReader::scanner() const
	{
		return this->_scanner;
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
}
