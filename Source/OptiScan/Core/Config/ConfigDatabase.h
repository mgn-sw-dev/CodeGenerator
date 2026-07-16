#pragma once

#include <OptiScan/Core/Common/Version.h>
#include <OptiScan/Core/Config/ConfigDatabaseCan.h>
#include <OptiScan/Core/Config/ConfigDatabaseLin.h>
#include <OptiScan/Core/Config/ConfigDatabaseStandardTrace.h>
#include <memory>
#include <string>
#include <vector>

namespace OptiScan::Core::Config
{
    class ConfigCanObject
    {
    public:
        std::vector<CanBusObject> _busses;
        std::optional<double> _frequencyMax_Hz;
        std::optional<std::string> _selectionTable;
        std::optional<StandardTrace> _standardTrace;
        std::optional<double> _xcpFrequencyMax_Hz;
        std::optional<std::string> _xcpSelectionTable;

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

    class ConfigLinObject
    {
    public:
        std::vector<LinBusObject> _busses;
        std::string _selectionTable;

        ConfigLinObject() = default;
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

        ConfigProjectInfos() = default;
    };

    class ConfigDatabase 
    {
    public:
        std::optional<ConfigCanObject> _canObject;
        ConfigDebugObject _debugObject;
        ConfigGpsObject _gpsObject;
        std::optional<ConfigLinObject> _linObject;
        ConfigProjectInfos _projectInfos;

        ConfigDatabase() = default;
    };



}
