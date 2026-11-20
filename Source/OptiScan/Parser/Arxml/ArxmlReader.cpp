
#include <OptiScan/Parser/Arxml/ArxmlReader.h>
#include <OptiScan/Parser/FileException.h>
#include <expat.h>
#include <utility>

using namespace std;

namespace OptiScan::Parser::Arxml
{
	const size_t ArxmlReader::BufferSize = 64 * 1024;

	ArxmlReader::ArxmlReader(istream * input)
		: _current()
		, _isFinished(false)
		, _input(input)
		, _pendingText()
		, _queue()
		, _parser(nullptr)
	{
		if (this->_input == nullptr)
		{
			throw InvalidOperationException("ArxmlReader: No input stream");
		}
		XML_Parser parser = XML_ParserCreate(nullptr);
		if (parser == nullptr)
		{
			throw InvalidOperationException("ArxmlReader: Could not create the XML parser");
		}
		this->_parser = parser;
		XML_SetUserData(parser, this);
		XML_SetElementHandler(parser, &ArxmlReader::onElementBegin, &ArxmlReader::onElementEnd);
		XML_SetCharacterDataHandler(parser, &ArxmlReader::onCharacterData);
	}

	ArxmlReader::~ArxmlReader()
	{
		if (this->_parser != nullptr)
		{
			XML_ParserFree(static_cast<XML_Parser>(this->_parser));
			this->_parser = nullptr;
		}
	}

	const string * ArxmlReader::attribute(const string & name) const
	{
		const string * result = nullptr;
		for (size_t i = 0; i < this->_current._attributes.size() && result == nullptr; i++)
		{
			if (this->_current._attributes[i]._name == name)
			{
				result = &this->_current._attributes[i]._value;
			}
		}
		return result;
	}

	void ArxmlReader::feed()
	{
		XML_Parser parser = static_cast<XML_Parser>(this->_parser);
		void * buffer = XML_GetBuffer(parser, static_cast<int>(ArxmlReader::BufferSize));
		if (buffer == nullptr)
		{
			throw InvalidOperationException("ArxmlReader: Could not allocate the input buffer");
		}
		this->_input->read(static_cast<char *>(buffer), static_cast<streamsize>(ArxmlReader::BufferSize));
		streamsize const count = this->_input->gcount();
		// A short read means the end of the stream. Treating a zero length read as
		// final as well keeps readNext from looping forever on a broken stream.
		bool const isFinal = count <= 0 || this->_input->eof();
		int isFinalFlag = 0;
		if (isFinal)
		{
			isFinalFlag = 1;
		}
		if (XML_ParseBuffer(parser, static_cast<int>(count), isFinalFlag) == XML_STATUS_ERROR)
		{
			this->throwExpatError();
		}
		if (isFinal)
		{
			this->flushPendingText();
			this->_isFinished = true;
		}
	}

	void ArxmlReader::flushPendingText()
	{
		if (!this->_pendingText.empty())
		{
			bool isWhitespaceOnly = true;
			for (size_t i = 0; i < this->_pendingText.size() && isWhitespaceOnly; i++)
			{
				char const value = this->_pendingText[i];
				isWhitespaceOnly = value == ' ' || value == '\t' || value == '\r' || value == '\n';
			}
			if (!isWhitespaceOnly)
			{
				ArxmlNode node;
				node._kind = ArxmlNodeKind::Text;
				node._text = std::move(this->_pendingText);
				node._lineNumber = static_cast<uint64_t>(
					XML_GetCurrentLineNumber64(static_cast<XML_Parser>(this->_parser)));
				this->_queue.push_back(std::move(node));
			}
			this->_pendingText.clear();
		}
	}

	ArxmlNodeKind ArxmlReader::getKind() const
	{
		return this->_current._kind;
	}

	uint64_t ArxmlReader::getLineNumber() const
	{
		return this->_current._lineNumber;
	}

	const string & ArxmlReader::getName() const
	{
		return this->_current._name;
	}

	void ArxmlReader::onCharacterData(void * userData, const char * text, int length)
	{
		ArxmlReader * reader = static_cast<ArxmlReader *>(userData);
		if (0 < length)
		{
			reader->_pendingText.append(text, static_cast<size_t>(length));
		}
	}

	void ArxmlReader::onElementBegin(void * userData, const char * name, const char ** attributes)
	{
		ArxmlReader * reader = static_cast<ArxmlReader *>(userData);
		reader->flushPendingText();
		ArxmlNode node;
		node._kind = ArxmlNodeKind::ElementBegin;
		node._name = name;
		node._lineNumber = static_cast<uint64_t>(
			XML_GetCurrentLineNumber64(static_cast<XML_Parser>(reader->_parser)));
		for (size_t i = 0; attributes[i] != nullptr && attributes[i + 1] != nullptr; i += 2)
		{
			ArxmlAttribute attribute;
			attribute._name = attributes[i];
			attribute._value = attributes[i + 1];
			node._attributes.push_back(std::move(attribute));
		}
		reader->_queue.push_back(std::move(node));
	}

	void ArxmlReader::onElementEnd(void * userData, const char * name)
	{
		ArxmlReader * reader = static_cast<ArxmlReader *>(userData);
		reader->flushPendingText();
		ArxmlNode node;
		node._kind = ArxmlNodeKind::ElementEnd;
		node._name = name;
		node._lineNumber = static_cast<uint64_t>(
			XML_GetCurrentLineNumber64(static_cast<XML_Parser>(reader->_parser)));
		reader->_queue.push_back(std::move(node));
	}

	bool ArxmlReader::readNext()
	{
		while (this->_queue.empty() && !this->_isFinished)
		{
			this->feed();
		}
		bool const result = !this->_queue.empty();
		if (result)
		{
			this->_current = std::move(this->_queue.front());
			this->_queue.pop_front();
		}
		else
		{
			this->_current = ArxmlNode();
		}
		return result;
	}

	const string & ArxmlReader::getText() const
	{
		return this->_current._text;
	}

	void ArxmlReader::throwExpatError() const
	{
		XML_Parser parser = static_cast<XML_Parser>(this->_parser);
		XML_Error const code = XML_GetErrorCode(parser);
		string message = "ArxmlReader (line ";
		message += to_string(XML_GetCurrentLineNumber64(parser));
		message += ", column ";
		message += to_string(XML_GetCurrentColumnNumber64(parser));
		message += "): ";
		message += XML_ErrorString(code);
		throw FormatException(message);
	}

}
