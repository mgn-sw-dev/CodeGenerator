
#include <OptiScan/Parser/A2l/McdIdent.h>

using namespace std;

namespace OptiScan::Parser::A2l
{
	McdIdent::McdIdent(const vector<McdIdentPart> & parts)
		: _parts(parts)
	{
	}

	string McdIdent::toString() const
	{
		string result;
		for (size_t i = 0; i < this->_parts.size(); i++)
		{
			if (0 < i)
			{
				result += '.';
			}
			result += (this->_parts[i].toString());
		}
		return result;
	}

	bool McdIdent::operator == (const McdIdent & other) const
	{
		return this->toString() == other.toString();
	}

	McdIdentPart::McdIdentPart(const string & name, const vector<McdArrayIndex> & arrayIndexList)
		: _arrayIndexList(arrayIndexList)
		, _name(name)
	{
	}

	string McdIdentPart::toString() const
	{
		string result = this->_name;
		for (const McdArrayIndex & arrayIndex : this->_arrayIndexList)
		{
			result += '[';
			visit([&result](auto && arg)
			{
				using T = decay_t<decltype(arg)>;
				if constexpr (is_same_v<T, string>)
				{
					result += arg;
				}
				else
				{
					result += to_string(arg);
				}
			}, arrayIndex);

			result += ']';
		}
		return result;
	}
}
