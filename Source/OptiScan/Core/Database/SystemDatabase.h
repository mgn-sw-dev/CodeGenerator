#pragma once

#include <OptiScan/Core/Config/SystemConfig.h>
#include <OptiScan/Core/Database/SelectionTable.h>
#include <filesystem>

namespace OptiScan::Core::Database
{

    class SystemDatabase
    {
    public:
        CanSelectionTable _canSelectionTable;
        LinSelectionTable _linSelectionTable;
        XcpSelectionTable _xcpSelectionTable;

        SystemDatabase();
        /** */
        void loadFromConfigFile(const std::string & configPath);
        /** */
        void loadSelectionTables();
        /** */
        void setLogHandler(LogHandler * logHandler);
    private:
        Config::ConfigDatabase _configDatabase;
        LogHandler * _logHandler;
        Config::SystemConfig _systemConfig;
        std::filesystem::path _systemPath;

        /** */
        void logError(const std::string & message) const;
        /** */
        void logInfo(const std::string & message) const;
    };

}