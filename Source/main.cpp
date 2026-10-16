#include <OptiScan/Core/Database/SystemDatabase.h>
#include <OptiScan/View/Console/ConsoleLogHandler.h>
#include <exception>
#include <iostream>
#include <string>

using namespace OptiScan::Core::Database;
using namespace OptiScan::View::Console;

int main(int argc, char * argv[])
{
	ConsoleLogHandler logHandler;
	SystemDatabase systemDatabase;
	systemDatabase.setLogHandler(&logHandler);
	try
	{
		systemDatabase.loadFromConfigFile("C:/CodeGenerator/Test/ConfigBevLongHaul.json");
		systemDatabase.loadSelectionTables();
		systemDatabase.loadVmms();
		systemDatabase.loadMapping();
	}
	catch (const std::exception & error)
	{
		logHandler.logError(std::string("CodeGenerator: ") + error.what());
		return 1;
	}
	return 0;
}
