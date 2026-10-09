#pragma once

#include <OptiScan/Parser/A2l/A2lTokenReader.h>

namespace OptiScan::Parser::A2l
{

	template <typename E>
	void A2lTokenReader::parseEnumValue(const std::map<std::string, E> & map, const std::string & name, E & value)
	{
		this->matchToken(A2lTokenKind::Identifier);
		const typename std::map<std::string, E>::const_iterator entry = map.find(this->token()._text);
		if (entry == map.end())
		{
			this->throwFormatError("A2lTokenReader::parseEnumValue",
				"Unknown " + name + " value '" + this->token()._text + "'");
		}
		value = entry->second;
		this->readNextToken();
	}

}
