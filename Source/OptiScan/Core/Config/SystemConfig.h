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
        /** Load Configuration File and validate against json schema.
         *  @param configPath: Path to the configuration json file.
         *  @throw runtime_error: from @ref loadConfigFile.
         *  @throw runtime_error: if validation failed. */
        void loadFromFile(const std::string & configPath);
        /**
         *  @throw runtime_error: If config file is not loaded.*/
        void parse(ConfigDatabase & configDatabase);
        /** */
        void setLogHandler(LogHandler * logHandler);
    private:
        nlohmann::json _jsonRootObject;
        LogHandler * _logHandler;

        /** Open file and parse as nlohmann::json object.
         * @throw runtime_error: If file failed to open.
         * @throw runtime_error: If JSON Parse Error. */
        void loadConfigFile(const std::string & configPath, nlohmann::json & jsonRootObject) const;
    	/** */
    	void log(const std::string & message) const;
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
    	/** Parse Hex String as UInt32 value.
    	 *  @throw runtime_error if conversion fails.
    	 *  @throw invalid_argument if hex string does not fit in uint32. */
    	static uint32_t parseHexStringAsUInt32(const std::string & hexString);
        /** */
        void parseCanBusses(const nlohmann::json & canObject, std::vector<std::unique_ptr<CanBusObject>> & canBusses);
        /** */
        void parseCanBusHandledMessagesObject(const nlohmann::json & jsonObject, CanHandledMessagesObject & canHandledMessages);
        /** */
        std::unique_ptr<CanBusObject> parseCanBusObject(const nlohmann::json & busElement);
        /** */
        void parseCanObject(ConfigDatabase & configDatabase);
    	/** */
    	void parseCanSignals(const nlohmann::json & signalsElement, CanTriggerSignal & canTriggerSignal);
    	/** Parse Attributs: BudRate, HardwareId, Name, Termination and Transmitting. */
    	void parseCommonCanBusFields(const nlohmann::json & busElement, CanBusObject & canBusObject);
        /** */
        void parseDebugObject(ConfigDatabase & configDatabase);
        /** */
        void parseGpsObject(ConfigDatabase & configDatabase);
    	/** */
    	void parseLinObject(ConfigDatabase & configDatabase);
    	/** */
    	std::unique_ptr<LinBusObject> parseLinBusObject(const nlohmann::json & busElement);
    	/** */
    	void parseLinBusses(const nlohmann::json & linObject, std::vector<std::unique_ptr<LinBusObject>> & linBusses);
        /** */
        void parseProjectInfoObject(ConfigDatabase & configDatabase);
    	/** Parse Attributs: DbcNames, HandledMessages. */
    	void parseStandardCanBusFields(const nlohmann::json & busElement, CanStandardBusObject & canBusObject);
    	/** */
    	void parseStandardTrace(const nlohmann::json & busElement, StandardTrace & standardTrace);
    	/** */
    	void parseTrigger(const nlohmann::json & triggerElement, Trigger & trigger);
        /** */
        void resetAllFields();
        /** */
        void validateAgainstSchema() const;


    };

}
