#pragma once
#include <OptiScan/Core/Config/ConfigDatabase.h>
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
         *  @param configPath: Path to the configuration json file.
         *  @throw runtime_error: For loading config file and validating config file. */
        void loadFromFile(const std::string & configPath);
        /**
         *  @throw runtime_error: If config file is not loaded.*/
        void parse(ConfigDatabase & configDatabase);
        /** */
        void setLogHandler(LogHandler * logHandler);
    private:
        nlohmann::json _jsonRootObject;
        LogHandler * _logHandler;

        /** */
        void loadConfigFile(const std::string & configPath, nlohmann::json & jsonRootObject) const;
        /** */
        void logError(const std::string & message) const;
        /** */
        void logInfo(const std::string & message) const;
        /** */
        void parseDebugObject(ConfigDatabase & configDatabase);
        /** */
        void parseGpsObject(ConfigDatabase & configDatabase);
        /** */
        void parseProjectInfoObject(ConfigDatabase & configDatabase);
        /** */
        void resetAllFields();
        /** */
        void validateAgainstSchema() const;


    };

}
