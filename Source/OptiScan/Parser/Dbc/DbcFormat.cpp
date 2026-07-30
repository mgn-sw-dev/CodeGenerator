
#include <OptiScan/Parser/Dbc/DbcFormat.h>

using namespace std;

namespace OptiScan::Parser::Dbc
{
	vector<DbcStringEscapeItem> const DbcFormat::StringEscapeItems =
	{
		{ '\\', '\\', false },
		{ '"', '"', false},
		// { 'n', '\n', true},
		// { 'r', '\r', true},
	};

	char const DbcFormat::StringEscapeStart('\\');

}
