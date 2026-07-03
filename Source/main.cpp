#include <OptiScan/Core/Config/SystemConfig.h>
#include <OptiScan/View/Console/ConsoleLogHandler.h>

int main(int argc, char *argv[])
{
	OptiScan::View::Console::ConsoleLogHandler logHandler;
    OptiScan::Core::Config::SystemConfig systemConfig;
	systemConfig.setLogHandler(&logHandler);
	systemConfig.loadFromFile("C:/CodeGenerator/Test/ConfigTest.json");
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
