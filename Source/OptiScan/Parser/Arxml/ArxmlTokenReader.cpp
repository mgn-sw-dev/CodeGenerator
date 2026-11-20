
#include <OptiScan/Parser/Arxml/ArxmlTokenReader.h>
#include <OptiScan/Parser/FileException.h>
#include <cerrno>
#include <cstdlib>
#include <limits>

using namespace std;

namespace OptiScan::Parser::Arxml
{
	namespace
	{
		const string EmptyString;

		/** Trim the whitespace that XML formatting leaves around a value. */
		string trimmed(const string & value)
		{
			size_t begin = 0;
			while (begin < value.size()
				&& (value[begin] == ' ' || value[begin] == '\t' || value[begin] == '\r' || value[begin] == '\n'))
			{
				begin++;
			}
			size_t end = value.size();
			while (begin < end
				&& (value[end - 1] == ' ' || value[end - 1] == '\t' || value[end - 1] == '\r' || value[end - 1] == '\n'))
			{
				end--;
			}
			return value.substr(begin, end - begin);
		}
	}

	ArxmlTokenReader::ArxmlTokenReader(istream * input)
		: _depth(0)
		, _hasNode(false)
		, _reader(input)
	{
	}

	const string * ArxmlTokenReader::attribute(const string & name) const
	{
		return this->_reader.attribute(name);
	}

	const string & ArxmlTokenReader::attributeOrEmpty(const string & name) const
	{
		const string * const result = this->_reader.attribute(name);
		return result == nullptr ? EmptyString : *result;
	}

	uint64_t ArxmlTokenReader::depth() const
	{
		return this->_depth;
	}

	bool ArxmlTokenReader::hasNode() const
	{
		return this->_hasNode;
	}

	ArxmlNodeKind ArxmlTokenReader::kind() const
	{
		return this->_reader.kind();
	}

	uint64_t ArxmlTokenReader::line() const
	{
		return this->_reader.line();
	}

	void ArxmlTokenReader::matchElementBegin(const string & name) const
	{
		if (!this->tryMatchElementBegin(name))
		{
			this->throwFormatError("ArxmlTokenReader::matchElementBegin",
				"Expected <" + name + ">, found " + ArxmlTokenReader::nodeKindToText(this->kind())
				+ (this->_hasNode && !this->_reader.name().empty() ? " '" + this->_reader.name() + "'" : ""));
		}
	}

	void ArxmlTokenReader::matchElementEnd(const string & name) const
	{
		if (!this->tryMatchElementEnd(name))
		{
			this->throwFormatError("ArxmlTokenReader::matchElementEnd",
				"Expected </" + name + ">, found " + ArxmlTokenReader::nodeKindToText(this->kind())
				+ (this->_hasNode && !this->_reader.name().empty() ? " '" + this->_reader.name() + "'" : ""));
		}
	}

	void ArxmlTokenReader::matchKind(ArxmlNodeKind kind) const
	{
		if (!this->_hasNode || this->_reader.kind() != kind)
		{
			this->throwFormatError("ArxmlTokenReader::matchKind",
				"Expected " + ArxmlTokenReader::nodeKindToText(kind)
				+ ", found " + ArxmlTokenReader::nodeKindToText(this->kind()));
		}
	}

	const string & ArxmlTokenReader::name() const
	{
		return this->_reader.name();
	}

	string ArxmlTokenReader::nodeKindToText(ArxmlNodeKind kind)
	{
		string result;
		switch (kind)
		{
		case ArxmlNodeKind::ElementBegin: result = "element begin"; break;
		case ArxmlNodeKind::ElementEnd:   result = "element end"; break;
		case ArxmlNodeKind::Text:         result = "text"; break;
		case ArxmlNodeKind::None:
		default:                          result = "end of document"; break;
		}
		return result;
	}

	void ArxmlTokenReader::parseElementBegin(const string & name)
	{
		this->matchElementBegin(name);
		this->readNext();
	}

	void ArxmlTokenReader::parseElementDouble(const string & name, double & value)
	{
		string text;
		this->parseElementTextChecked(name, "ArxmlTokenReader::parseElementDouble", text);
		const char * const begin = text.c_str();
		char * end = nullptr;
		errno = 0;
		double const parsed = strtod(begin, &end);
		if (end == begin || *end != '\0' || errno == ERANGE)
		{
			this->throwFormatError("ArxmlTokenReader::parseElementDouble",
				"<" + name + "> is not a number: '" + text + "'");
		}
		value = parsed;
	}

	void ArxmlTokenReader::parseElementEnd(const string & name)
	{
		this->matchElementEnd(name);
		this->readNext();
	}

	void ArxmlTokenReader::parseElementInt64(const string & name, int64_t & value)
	{
		string text;
		this->parseElementTextChecked(name, "ArxmlTokenReader::parseElementInt64", text);
		const char * const begin = text.c_str();
		char * end = nullptr;
		errno = 0;
		long long const parsed = strtoll(begin, &end, 10);
		if (end == begin || *end != '\0' || errno == ERANGE)
		{
			this->throwFormatError("ArxmlTokenReader::parseElementInt64",
				"<" + name + "> is not an integer: '" + text + "'");
		}
		value = static_cast<int64_t>(parsed);
	}

	void ArxmlTokenReader::parseElementText(const string & name, string & value)
	{
		this->parseElementTextChecked(name, "ArxmlTokenReader::parseElementText", value);
	}

	void ArxmlTokenReader::parseElementTextChecked(const string & name, const string & context, string & value)
	{
		this->matchElementBegin(name);
		this->readNext();
		value.clear();
		if (this->_hasNode && this->_reader.kind() == ArxmlNodeKind::Text)
		{
			value = trimmed(this->_reader.text());
			this->readNext();
		}
		if (!this->tryMatchElementEnd(name))
		{
			this->throwFormatError(context,
				"<" + name + "> was expected to have text content only, found "
				+ ArxmlTokenReader::nodeKindToText(this->kind())
				+ (this->_hasNode && !this->_reader.name().empty() ? " '" + this->_reader.name() + "'" : ""));
		}
		this->readNext();
	}

	void ArxmlTokenReader::parseElementUInt16(const string & name, uint16_t & value)
	{
		uint64_t tmp = 0;
		this->parseElementUInt64(name, tmp);
		if (numeric_limits<uint16_t>::max() < tmp)
		{
			this->throwFormatError("ArxmlTokenReader::parseElementUInt16", "<" + name + "> is not an uint16");
		}
		value = static_cast<uint16_t>(tmp);
	}

	void ArxmlTokenReader::parseElementUInt32(const string & name, uint32_t & value)
	{
		uint64_t tmp = 0;
		this->parseElementUInt64(name, tmp);
		if (numeric_limits<uint32_t>::max() < tmp)
		{
			this->throwFormatError("ArxmlTokenReader::parseElementUInt32", "<" + name + "> is not an uint32");
		}
		value = static_cast<uint32_t>(tmp);
	}

	void ArxmlTokenReader::parseElementUInt64(const string & name, uint64_t & value)
	{
		string text;
		this->parseElementTextChecked(name, "ArxmlTokenReader::parseElementUInt64", text);
		if (text.empty() || text[0] == '-')
		{
			this->throwFormatError("ArxmlTokenReader::parseElementUInt64",
				"<" + name + "> is not an unsigned integer: '" + text + "'");
		}
		const char * const begin = text.c_str();
		char * end = nullptr;
		errno = 0;
		unsigned long long const parsed = strtoull(begin, &end, 10);
		if (end == begin || *end != '\0' || errno == ERANGE)
		{
			this->throwFormatError("ArxmlTokenReader::parseElementUInt64",
				"<" + name + "> is not an unsigned integer: '" + text + "'");
		}
		value = static_cast<uint64_t>(parsed);
	}

	bool ArxmlTokenReader::readNext()
	{
		// The depth is tracked before the node is handed out, so depth() reports the
		// level the current node sits on: an ElementBegin raises it, the matching
		// ElementEnd lowers it again.
		if (this->_hasNode && this->_reader.kind() == ArxmlNodeKind::ElementEnd && 0 < this->_depth)
		{
			this->_depth--;
		}
		this->_hasNode = this->_reader.readNext();
		if (this->_hasNode && this->_reader.kind() == ArxmlNodeKind::ElementBegin)
		{
			this->_depth++;
		}
		return this->_hasNode;
	}

	void ArxmlTokenReader::skipElement()
	{
		this->matchKind(ArxmlNodeKind::ElementBegin);
		string const name = this->_reader.name();
		// The closing node of this element is reported at the same depth as the opening
		// one, because readNext lowers the depth only when it moves past the closing
		// node. Children therefore always end one level deeper.
		uint64_t const target = this->_depth;
		this->readNext();
		while (this->_hasNode
			&& !(this->_reader.kind() == ArxmlNodeKind::ElementEnd && this->_depth == target))
		{
			this->readNext();
		}
		if (!this->_hasNode)
		{
			this->throwFormatError("ArxmlTokenReader::skipElement",
				"Unexpected end of document inside <" + name + ">");
		}
		this->readNext();
	}

	const string & ArxmlTokenReader::text() const
	{
		return this->_reader.text();
	}

	void ArxmlTokenReader::throwFormatError(const string & context, const string & message) const
	{
		string position;
		if (this->_hasNode)
		{
			position = " (line " + to_string(this->_reader.line()) + ")";
		}
		throw FormatException(context + position + ": " + message);
	}

	bool ArxmlTokenReader::tryMatchElementBegin(const string & name) const
	{
		return this->_hasNode
			&& this->_reader.kind() == ArxmlNodeKind::ElementBegin
			&& this->_reader.name() == name;
	}

	bool ArxmlTokenReader::tryMatchElementEnd(const string & name) const
	{
		return this->_hasNode
			&& this->_reader.kind() == ArxmlNodeKind::ElementEnd
			&& this->_reader.name() == name;
	}

}
