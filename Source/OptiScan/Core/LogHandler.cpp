#include <chrono>
#include <OptiScan/Core/LogHandler.h>
#include <iomanip>
#include <sstream>

using namespace std;

namespace OptiScan::Core
{

	string LogHandler::boolToString(bool value)
	{
		string result = "false";
		if (value)
		{
			result = "true";
		}
		return result;
	}

	std::string LogHandler::doubleToString(double value, int precision)
	{
		ostringstream oss;
		oss << fixed << setprecision(precision) << value;
		return oss.str();
	}

}
