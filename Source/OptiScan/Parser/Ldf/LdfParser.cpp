
#include <OptiScan/Parser/Ldf/LdfParser.h>

#include "LdfKeyword.h"
#include "OptiScan/Parser/TokenReaderUtils.h"

using namespace std;

namespace OptiScan::Parser::Ldf
{
	LdfParser::LdfParser(istream * input)
		: _modes(Mode::None)
		, _reader(input)
	{
	}

	void LdfParser::parse(LdfDatabase & ldfDatabase)
	{
		ldfDatabase = LdfDatabase();
		this->_reader.readNextToken();
		this->parseLinDescriptionFile();
		this->parseLinProtocolVersion(ldfDatabase._protocolVersion);
		this->parseLinLanguageVersion(ldfDatabase._languageVersion);

	}

	void LdfParser::parseLinDescriptionFile()
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::LinDescriptionFile);
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
	}

	void LdfParser::parseLinLanguageVersion(string & languageVersion)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::LinLanguageVersion);
		this->parseLinVersions(languageVersion);
	}

	void LdfParser::parseLinProtocolVersion(string & protocolVersion)
	{
		this->_reader.matchKeywordAndRead(LdfKeyword::LinProtocolVersion);
		this->parseLinVersions(protocolVersion);
	}

	void LdfParser::parseLinVersions(std::string & version)
	{
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorEqualSign);
		this->_reader.matchToken(LdfTokenKind::LiteralString);
		version = TokenReaderUtils::literalStringTokenTextToString(this->_reader.token()._text);
		this->_reader.readNextToken();
		this->_reader.matchTokenAndRead(LdfTokenKind::OperatorSemicolon);
	}
}
