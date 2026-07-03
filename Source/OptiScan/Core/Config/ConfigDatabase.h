#pragma once

#include <OptiScan/Core/Common/Version.h>
#include <string>

namespace OptiScan::Core::Config
{
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
        ConfigProjectInfos _projectInfos;

        ConfigDatabase();
    };



}
