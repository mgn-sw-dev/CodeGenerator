
#include <OptiScan/Parser/Arxml/ArxmlKeyword.h>
#include <OptiScan/Parser/Arxml/ArxmlParser.h>
#include <OptiScan/Parser/FileException.h>
#include <utility>

using namespace std;

namespace OptiScan::Parser::Arxml
{
	ArxmlParser::ArxmlParser(istream * input)
		: _reader(input)
	{
	}

	void ArxmlParser::parse(ArxmlDatabase & arxmlDatabase)
	{
		arxmlDatabase = ArxmlDatabase();
		if (!this->_reader.readNext())
		{
			this->_reader.throwFormatError("ArxmlParser::parse", "The document is empty");
		}
		this->_reader.matchElementBegin(ArxmlKeyword::Autosar);
		this->parseElement(arxmlDatabase, string());
		if (this->_reader.hasNode())
		{
			this->_reader.throwFormatError("ArxmlParser::parse",
				"Unexpected content after </" + ArxmlKeyword::Autosar + ">");
		}
	}

	void ArxmlParser::parseElement(ArxmlDatabase & arxmlDatabase, const string & parentPath)
	{
		this->_reader.matchKind(ArxmlNodeKind::ElementBegin);
		string const elementType = this->_reader.getName();
		uint64_t const lineNumber = this->_reader.getLineNumber();
		if (elementType == ArxmlKeyword::AdminData)
		{
			// Pure metadata: SDG, SD and the captions below them. Their SHORT-NAMEs are
			// not referenceable, so indexing them would pollute the path index.
			this->_reader.skipElement();
		}
		else
		{
			this->_reader.readNext();
			string path = parentPath;
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::ShortName))
			{
				// In AUTOSAR the SHORT-NAME is the first child of an identifiable element.
				string shortName;
				this->_reader.parseElementText(ArxmlKeyword::ShortName, shortName);
				path = parentPath + "/" + shortName;
				this->registerPath(arxmlDatabase, path, elementType, lineNumber);
			}
			while (!this->_reader.tryMatchElementEnd(elementType))
			{
				if (!this->_reader.hasNode())
				{
					this->_reader.throwFormatError("ArxmlParser::parseElement",
						"Unexpected end of document inside <" + elementType + ">");
				}
				if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
				{
					this->parseElement(arxmlDatabase, path);
				}
				else
				{
					this->_reader.readNext();
				}
			}
			this->_reader.readNext();
		}
	}

	void ArxmlParser::registerPath(ArxmlDatabase & arxmlDatabase, const string & path,
		const string & elementType, uint64_t lineNumber)
	{
		ArxmlIdentifiable entry;
		entry._elementType = elementType;
		entry._lineNumber = lineNumber;
		pair<map<string, ArxmlIdentifiable>::iterator, bool> const inserted =
			arxmlDatabase._pathIndex.insert(make_pair(path, std::move(entry)));
		if (!inserted.second)
		{
			// AUTOSAR requires paths to be unique. The first occurrence is kept so a
			// broken file still yields a usable index.
			arxmlDatabase._duplicatePaths.push_back(path);
		}
	}

}
