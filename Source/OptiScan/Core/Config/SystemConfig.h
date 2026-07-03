#pragma once
#include <Extern/Nlohmann/json.hpp>
#include <OptiScan/Core/LogHandler.h>
#include <string>

namespace OptiScan::Core::Config
{
    class SystemConfig 
    {
    public:
        SystemConfig();
        /** Load Configuration File.
         *  @param configPath: Path to the configuration json file. */
        bool loadFromFile(const std::string & configPath);
        /** */
        void setLogHandler(LogHandler * logHandler);
    private:
        LogHandler * _logHandler;

        /** */
        void loadConfigFile(const std::string & configPath, nlohmann::json & jsonRootObject) const;
        /** */
        void logError(const std::string & message) const;
        /** */
        void logInfo(const std::string & message) const;
        /** */
        void resetAllFields();
        /** */
        void validateAgainstSchema(const nlohmann::json & jsonRootObject) const;


    };

}
