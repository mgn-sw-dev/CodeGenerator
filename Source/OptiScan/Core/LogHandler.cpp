#include <chrono>
#include <OptiScan/Core/LogHandler.h>
#include <iomanip>
#include <sstream>

using namespace std;

namespace OptiScan::Core
{
	std::string LogHandler::doubleToString(double value, int precision)
	{
		ostringstream oss;
		oss << fixed << setprecision(precision) << value;
		return oss.str();
	}

	std::string LogHandler::exeptionToString(const std::exception & e)
	{
		stringstream ss;
		ss << e.what();
		return ss.str();
	}

}
