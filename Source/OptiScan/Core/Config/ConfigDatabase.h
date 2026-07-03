#pragma once

#include <OptiScan/Core/Common/Version.h>
#include <string>

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
        ConfigDebugObject _debugObject;
        ConfigGpsObject _gpsObject;
        ConfigProjectInfos _projectInfos;

        ConfigDatabase();
    };



}
