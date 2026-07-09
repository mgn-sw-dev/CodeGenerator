#pragma once

#include <OptiScan/Core/Common/Version.h>
#include <OptiScan/Core/Config/ConfigDatabaseCan.h>
#include <memory>
#include <string>
#include <vector>

namespace OptiScan::Core::Config
{
    class ConfigCanObject
    {
    public:
        std::vector<std::unique_ptr<CanBusObject>> _busses = {};
        std::optional<double> _frequencyMax_Hz = std::nullopt;
        std::optional<std::string> _selectionTable = std::nullopt;
        std::optional<double> _xcpFrequencyMax_Hz = std::nullopt;
        std::optional<std::string> _xcpSelectionTable = std::nullopt;

        ConfigCanObject() = default;
    };

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
        std::optional<ConfigCanObject> _canObject;
        ConfigDebugObject _debugObject;
        ConfigGpsObject _gpsObject;
        ConfigProjectInfos _projectInfos;

        ConfigDatabase();
    };



}
