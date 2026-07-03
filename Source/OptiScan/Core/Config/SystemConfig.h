#pragma once
#include <functional>
#include <string>

#include <Extern/Nlohmann/json.hpp>
#include <OptiScan/Core/LogHandler.h>

namespace OptiScan::Core::Config
{
    class SystemConfig 
    {
    public:
        SystemConfig();
        /** Load Configuration File.
         *  @param configPath: Path to the configuration json file.
         *  @todo param log handler. */
        bool loadFromFile(const std::string & configPath);
        /** */
        void setLogHandler(LogHandler * logHandler);
    private:
        LogHandler * _logHandler;

        /** */
        void logInfo(const std::string & message) const;
        /** */
        void resetAllFields();


    };

}
