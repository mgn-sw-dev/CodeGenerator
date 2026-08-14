
#include <OptiScan/Parser/A2l/A2lKeyword.h>
#include <OptiScan/Parser/A2l/A2lParser.h>

using namespace std;

namespace OptiScan::Parser::A2l
{
	A2lParser::A2lParser(std::istream * input)
		: _reader(input)
	{
	}

	void A2lParser::parse(A2lDatabase & a2lDatabase)
	{
		a2lDatabase = A2lDatabase();
		this->_reader.readNextToken();
		this->parseAsap2Version(a2lDatabase._asap2Version);
		this->parseA2mlVersion(*a2lDatabase._a2mlVersion);
	}

	void A2lParser::parseA2mlVersion(McdVersion & version)
	{
		if (this->_reader.tryMatchKeyword(A2lKeyword::A2mlVersion))
		{
			this->_reader.readNextToken();
			this->parseVersion(version);
		}
	}

	void A2lParser::parseAsap2Version(McdVersion & version)
	{
		this->_reader.matchKeyword(A2lKeyword::Asap2Version);
		this->_reader.readNextToken();
		this->parseVersion(version);
	}

	void A2lParser::parseVersion(McdVersion & version)
	{
		this->_reader.parseUInt16(version._versionNo);
		this->_reader.parseUInt16(version._upgradeNo);
	}
}
