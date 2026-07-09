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
         * @param messageSignalMaps: vector of CanMessageSignalMap */
        static void parseArrayAsMessageSignalMap(const nlohmann::json & array, std::vector<CanMessageSignalMap> & messageSignalMaps);
        /** Parse Json Array elementwise. Each element is expected to be a string.
         * Push string into strings.
         * @param array: Json Array
         * @param strings: vector of string */
        static void parseArrayAsString(const nlohmann::json & array, std::vector<std::string> & strings);
        /** */
        void parseCanBusses(const nlohmann::json & canObject, std::vector<std::unique_ptr<CanBusObject>> & canBusses);
        /** */
        void parseCanBusHandledMessagesObject(const nlohmann::json & jsonObject, CanHandledMessagesObject & canHandledMessages);
        /** */
        std::unique_ptr<CanBusObject> parseCanBusObject(const nlohmann::json & busElement);
        /** */
        void parseCanObject(ConfigDatabase & configDatabase);
    	/** Parse Attributs: BudRate, HardwareId, Name, Termination and Transmitting. */
    	void parseCommonCanBusFields(const nlohmann::json & busElement, CanBusObject & canBusObject);
        /** */
        void parseDebugObject(ConfigDatabase & configDatabase);
        /** */
        void parseGpsObject(ConfigDatabase & configDatabase);
        /** */
        void parseProjectInfoObject(ConfigDatabase & configDatabase);
    	/** Parse Attributs: DbcNames, HandledMessages. */
    	void parseStandardCanBusFields(const nlohmann::json & busElement, CanStandardBusObject & canBusObject);
        /** */
        void resetAllFields();
        /** */
        void validateAgainstSchema() const;


    };

}
