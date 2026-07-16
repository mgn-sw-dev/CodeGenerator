
#include <OptiScan/View/Console/ConsoleLogHandler.h>
#include <iostream>

using namespace std;

namespace OptiScan::View::Console
{
	void ConsoleLogHandler::log(const string & message)
	{
		cout << message << endl;
	}

	void ConsoleLogHandler::logError(const string & message)
	{
		cerr << "[ERROR] " << message << endl;
	}

	void ConsoleLogHandler::logInfo(const string & message)
	{
		cout << "[INFO] " << message << endl;
	}

	void ConsoleLogHandler::logWarning(const string & message)
	{
		cout << "[WARNING] " << message << endl;
	}
}
