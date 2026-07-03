#pragma once
#include <functional>
#include <string>

#include <Extern/Nlohmann/json.hpp>

namespace OptiScan::Core::Config
{
    class SystemConfig 
    {
    public:
        using LogHandler = std::function<void(const std::string&)>;

        SystemConfig();
        /** Load Configuration File.
         *  @param configPath: Path to the configuration json file.
         *  @todo param log handler. */
        bool loadFromFile(const std::string & configPath);
        /** */
        void setLogHandler(LogHandler logHandler);
    private:
        LogHandler _logHandler;

        /** */
        void log(const std::string & message) const;
        /** */
        void resetAllFields();


    };

}
