
#include <OptiScan/Core/Config/ConfigDatabase.h>

using namespace std;

namespace OptiScan::Core::Config
{
	ConfigDatabase::ConfigDatabase()
		: _projectInfos()
	{
	}

	ConfigProjectInfos::ConfigProjectInfos()
		: _customer()
		, _firmwareVersion()
		, _fleetManagementRelease()
		, _generation()
		, _projectName()
		, _systemName()
		, _systemVersion()
	{
	}
}
