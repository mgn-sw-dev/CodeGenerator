#pragma once

#include <OptiScan/Core/Config/SystemConfig.h>
#include <OptiScan/Core/Database/CanDatabase.h>
#include <OptiScan/Core/Database/LinDatabase.h>
#include <OptiScan/Core/Database/SelectionTable.h>
#include <OptiScan/Core/Database/SystemMapping.h>
#include <filesystem>

namespace OptiScan::Core::Database
{

    class SystemDatabase
    {
    public:
        std::vector<CanDatabase> _canDatabases;
        CanSelectionTable _canSelectionTable;
        std::vector<LinDatabase> _linDatabases;
        LinSelectionTable _linSelectionTable;
        SystemMapping _mapping;
        XcpSelectionTable _xcpSelectionTable;

        SystemDatabase();
        /** */
        void loadMapping();
        /** */
        void loadFromConfigFile(const std::string & configPath);
        /** Load Json selection tables for can, lin and xcp if
         *  signallist is set in config.
         *  Catch errors from SelectionTable. */
        void loadSelectionTables();
        /** Load Vmms for can, lin and xcp.
          * Supported file types: dbc, a2l, ldf. */
        void loadVmms();
        /** */
        void setLogHandler(LogHandler * logHandler);
    private:
        Config::ConfigDatabase _configDatabase;
        LogHandler * _logHandler;
        Config::SystemConfig _systemConfig;
        std::filesystem::path _systemPath;

        void loadA2lDatabase(const Config::CanBusObject & canBus);
        /** */
        void loadDbcDatabase(const Config::CanBusObject & canBus);
        /** */
        void loadLdfDatabase(const Config::LinBusObject & linBus, const Parser::Ldf::LdfModes & ldfModes);
        /** */
        void log(const std::string & message) const;
        /** */
        void logError(const std::string & message) const;
        /** */
        void logInfo(const std::string & message) const;
    };

}