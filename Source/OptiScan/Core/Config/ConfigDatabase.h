#pragma once

#include <string>

namespace OptiScan::Core::Config
{
    class ConfigProjectInfos
    {
    public:
        std::string _customer;
        // ToDo: Change Type to Version
        std::string _firmwareVersion;
        std::string _fleetManagementRelease;
        uint8_t _generation;
        std::string _projectName;
        std::string _systemName;
        // ToDo: Change Type to Version
        std::string _systemVersion;

        ConfigProjectInfos();
    };

    class ConfigDatabase 
    {
    public:
        ConfigProjectInfos _projectInfos;

        ConfigDatabase();
    };



}
