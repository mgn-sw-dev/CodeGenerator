
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
	}
}
