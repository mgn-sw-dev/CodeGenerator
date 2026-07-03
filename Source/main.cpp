#include <OptiScan/Core/Config/SystemConfig.h>
#include <iostream>

int main(int argc, char *argv[])
{
    OptiScan::Core::Config::SystemConfig systemConfig;
	systemConfig.setLogHandler([](const std::string & message)
	{
		std::cout << message << std::endl;
	});
	systemConfig.loadFromFile("config.json");
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
