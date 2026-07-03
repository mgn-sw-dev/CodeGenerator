#pragma once

#include <OptiScan/Core/Common/Version.h>
#include <OptiScan/Core/Config/ConfigDatabaseCan.h>
#include <memory>
#include <string>
#include <vector>

namespace OptiScan::Core::Config
{
    class ConfigDebugObject
    {
    public:
        double _frequency_Hz = 0.0;
        bool _vinIncluded = false;

        ConfigDebugObject() = default;
    };

    class ConfigGpsObject
    {
    public:
        double _frequency_Hz = 0.0;

        ConfigGpsObject() = default;
    };

    class ConfigProjectInfos
    {
    public:
        std::string _customer;
        Common::Version _firmwareVersion;
        std::string _fleetManagementRelease;
        uint8_t _generation;
        std::string _projectName;
        std::string _systemName;
        Common::Version _systemVersion;

        ConfigProjectInfos();
    };

    class ConfigDatabase 
    {
    public:
        std::vector<std::unique_ptr<CanBusObject>> _canBusObjects;
        ConfigDebugObject _debugObject;
        ConfigGpsObject _gpsObject;
        ConfigProjectInfos _projectInfos;

        ConfigDatabase();
    };



}
