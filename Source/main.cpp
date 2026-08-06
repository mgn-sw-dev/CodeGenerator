#include <OptiScan/Core/Database/SystemDatabase.h>
#include <OptiScan/View/Console/ConsoleLogHandler.h>
#include <OptiScan/Core/Database/CanDatabase.h>

int main(int argc, char *argv[])
{
	OptiScan::View::Console::ConsoleLogHandler logHandler;
	OptiScan::Core::Database::CanDatabase canDatabase;
	canDatabase.loadDbcDatabase("C:/CodeGenerator/Test/240102BatteryCAN.dbc");
	OptiScan::Core::Database::SystemDatabase systemDatabase;
	systemDatabase.setLogHandler(&logHandler);
	systemDatabase.loadFromConfigFile("C:/CodeGenerator/Test/ConfigBevLongHaul.json");
	systemDatabase.loadSelectionTables();
	systemDatabase.loadMapping();
	systemDatabase.loadVmms();
	return 0;

#if 0
	Q_INIT_RESOURCE(OptiScanCoreCodeGenerator);
	QApplication a(argc, argv);
	a.setApplicationName(VERSION_PRODUCT);
	a.setOrganizationName(VERSION_ORGANIZATION);
	QString version = QVersionNumber(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION).toString();
#ifdef QT_DEBUG
	version.append("-debug");
#endif
	a.setApplicationVersion(version);
	MainModel model;
	MainWindow w(&model);
	w.show();
	return a.exec();
#endif
}
