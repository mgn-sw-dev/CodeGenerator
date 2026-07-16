#pragma once

#include <OptiScan/Core/Config/SystemConfig.h>

namespace OptiScan::Core::Database
{

    class SystemDatabase
    {
    public:
        SystemDatabase();
        /** */
        void loadFromConfigFile(const std::string & configPath);
        /** */
        void setLogHandler(LogHandler * logHandler);
    private:
        Config::ConfigDatabase _configDatabase;
        LogHandler * _logHandler;
        Config::SystemConfig _systemConfig;
    };

}