
#include <OptiScan/Parser/Ldf/LdfParser.h>

using namespace std;

namespace OptiScan::Parser::Ldf
{
	LdfParser::LdfParser(istream * input)
		: _modes(Mode::None)
		, _scanner(input)
	{
	}
}
