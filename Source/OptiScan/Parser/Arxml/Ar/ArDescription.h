#pragma once

#include <string>

namespace OptiScan::Parser::Arxml::Ar
{
	/** DESC element: the free text description, held per language.
	 *
	 *  A DBC carries a single comment per signal, so these are the texts a later
	 *  translation can draw from. Only the languages actually used in the reference
	 *  file are modelled; anything else is read and dropped. */
	struct ArDescription
	{
		std::string _english;
		std::string _german;
	};
}
