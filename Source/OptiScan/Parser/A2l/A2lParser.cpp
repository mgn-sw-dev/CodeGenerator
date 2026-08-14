
#include <OptiScan/Parser/A2l/A2lKeyword.h>
#include <OptiScan/Parser/A2l/A2lParser.h>
#include <OptiScan/Parser/FileException.h>
#include <OptiScan/Parser/TokenReaderUtils.h>
#include <unordered_set>

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
		this->parseA2mlVersion(a2lDatabase._a2mlVersion);
		this->parseProject(a2lDatabase._project);

		// end of file, do uncomment if all parsing methods available
		// this->_reader.matchToken(A2lTokenKind::None);
	}

	void A2lParser::parseA2mlVersion(optional<McdVersion> & version)
	{
		if (this->_reader.tryMatchKeyword(A2lKeyword::A2mlVersion))
		{
			this->_reader.readNextToken();
			version.emplace();
			this->parseVersion(*version);
		}
	}

	void A2lParser::parseAsap2Version(McdVersion & version)
	{
		this->_reader.matchKeyword(A2lKeyword::Asap2Version);
		this->_reader.readNextToken();
		this->parseVersion(version);
	}

	void A2lParser::parseBlockBegin(const string & keyword)
	{
		this->_reader.matchToken(A2lTokenKind::EscapeBegin);
		this->_reader.readNextToken();
		this->_reader.matchKeyword(keyword);
		this->_reader.readNextToken();
	}

	void A2lParser::parseBlockEnd(const string & keyword)
	{
		this->_reader.matchToken(A2lTokenKind::EscapeEnd);
		this->_reader.readNextToken();
		this->_reader.matchKeyword(keyword);
		this->_reader.readNextToken();
	}

	void A2lParser::parseIdent(McdIdent & ident)
	{
		bool checkNext = true;
		while (checkNext)
		{
			McdIdentPart part;
			this->parseIdentPart(part);
			ident._parts.push_back(part);
			checkNext = this->_reader.tryMatchToken(A2lTokenKind::Dot);
			if (checkNext)
			{
				this->_reader.readNextToken();
			}
		}
	}

	void A2lParser::parseIdentPart(McdIdentPart & part)
	{
		this->_reader.matchToken(A2lTokenKind::Identifier);
		part._name = this->_reader.token()._text;
		this->_reader.readNextToken();
		while (this->_reader.tryMatchToken(A2lTokenKind::LeftSquareBracket))
		{
			this->_reader.readNextToken();
			if (this->_reader.tryMatchToken(A2lTokenKind::Identifier))
			{
				part._arrayIndexList.push_back(this->_reader.token()._text);
			}
			else if (this->_reader.tryMatchToken(A2lTokenKind::IntegerLiteral))
			{
				part._arrayIndexList.push_back(TokenReaderUtils::literalIntegerTokenTextToInt64(this->_reader.token()._text));
			}
			else
			{
				throw FormatException("A2lParser:parseIdentPart: Expected identifier or integer literal");
			}
			this->_reader.readNextToken();
			this->_reader.matchToken(A2lTokenKind::RightSquareBracket);
			this->_reader.readNextToken();
		}
	}

	void A2lParser::parseProject(McdProject & project)
	{
		this->parseBlockBegin(A2lKeyword::Project);
		this->parseIdent(project._name);
		this->_reader.parseString(project._longIdentifier);
		string keyword;
		unordered_set<string> oneTimeKeywords;
		while (this->_reader.tryTokenStackBlockBeginAny(keyword))
		{
			if (keyword == A2lKeyword::Header)
			{
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				this->parseHeader();
			}
			else
			{
				this->parseModule();
			}
		}
		this->parseBlockEnd(A2lKeyword::Project);

	}

	void A2lParser::parseVersion(McdVersion & version)
	{
		this->_reader.parseUInt16(version._versionNo);
		this->_reader.parseUInt16(version._upgradeNo);
	}
}
