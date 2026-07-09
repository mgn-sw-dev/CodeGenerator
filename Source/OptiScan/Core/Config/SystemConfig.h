#pragma once
#include <OptiScan/Core/Config/ConfigDatabase.h>
#include <Extern/Nlohmann/json.hpp>
#include <OptiScan/Core/LogHandler.h>
#include <memory>
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
        /** Parse Json Array elementwise. Each element is expected to be a CanMessageSignalMap.
         * Push CanMessageSignalMap into messageSignalMap.
         * @param array: Json Array
         * @param messageSignalMap: vector of CanMessageSignalMap */
        static void parseArrayAsMessageSignalMap(const nlohmann::json & array, std::vector<CanMessageSignalMap> & messageSignalMap);
        /** */
        void parseCanBusses(ConfigDatabase & configDatabase, const nlohmann::json & canObject);
        /** */
        void parseCanBusDbcArray(const nlohmann::json & dbcArray, std::vector<std::string> & dbcNames);
        /** */
        void parseCanBusHandledMessagesObject(const nlohmann::json & jsonObject, CanHandledMessagesObject & canHandledMessages);
        /** */
        std::unique_ptr<CanBusObject> parseCanBusObject(const nlohmann::json & busElement);
        /** */
        void parseCanObject(ConfigDatabase & configDatabase);
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
