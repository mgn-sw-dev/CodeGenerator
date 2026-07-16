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
        /** @throw runtime_error: If config file is not loaded.*/
        void parse(ConfigDatabase & configDatabase);
        /** */
        void setLogHandler(LogHandler * logHandler);
    private:
        nlohmann::json _jsonRootObject;
        LogHandler * _logHandler;

        /** Open file and parse as nlohmann::json object.
         * @throw runtime_error: If file failed to open.
         * @throw runtime_error: If JSON Parse Error. */
        static void loadConfigFile(const std::string & configPath, nlohmann::json & jsonRootObject);
    	/** */
    	void log(const std::string & message) const;
        /** */
        void logError(const std::string & message) const;
        /** */
        void logInfo(const std::string & message) const;
    	/** Parse Json array elementwise.*/
    	template<typename T>
    	static void parseArrayAs(const nlohmann::json & array, std::vector<T> & target);
    	/** */
    	template<typename T, typename Parser>
    	static void parseArrayWith(const nlohmann::json & array, std::vector<T> & target, Parser parser);
    	/** Parse Hex String as UInt32 value.
    	 *  @throw runtime_error if conversion fails.
    	 *  @throw invalid_argument if hex string does not fit in uint32. */
    	static uint32_t parseHexStringAsUInt32(const std::string & hexString);
        /** Check and parse can handled massage object properties GPS and VIN. */
        static void parseCanBusHandledMessagesObject(const nlohmann::json & jsonObject, CanHandledMessagesObject & canHandledMessages);
    	/** Select by type of can object with properties need to parse.
    	 * @throw runtime_error if can bus object is invalid. */
        static void parseCanBusObject(const nlohmann::json & busElement, CanBusObject & canBusObject);
    	/** */
    	static void parseCanMessageSignalMap(const nlohmann::json & signalElement, CanMessageSignalMap & canMessageSignalMap);
        /** */
        void parseCanObject(ConfigDatabase & configDatabase);
    	/** */
    	void parseCanSignals(const nlohmann::json & signalsElement, CanTriggerSignal & canTriggerSignal);
    	/** Parse Attributs: BudRate, HardwareId, Name, Termination and Transmitting. */
    	static void parseCommonCanBusFields(const nlohmann::json & busElement, CanBusObject & canBusObject);
        /** */
        void parseDebugObject(ConfigDatabase & configDatabase);
        /** */
        void parseGpsObject(ConfigDatabase & configDatabase);
    	/** */
    	void parseLinObject(ConfigDatabase & configDatabase);
    	/** */
    	static void parseLinBusObject(const nlohmann::json & busElement, LinBusObject & linBusObject);
    	/** */
    	template <typename T>
    	static void parseOptionalValue(const nlohmann::json & json, const std::string & key, std::optional<T> & target);
        /** */
        void parseProjectInfoObject(ConfigDatabase & configDatabase);
    	/** Parse Attributs: DbcNames, HandledMessages. */
    	static void parseStandardCanBusFields(const nlohmann::json & busElement, CanBusObject & canBusObject);
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
#include <OptiScan/Core/Config/SystemConfig.inl.h>

