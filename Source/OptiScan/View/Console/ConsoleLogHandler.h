#pragma once

#include <OptiScan/Core/LogHandler.h>

namespace OptiScan::View::Console
{
    class ConsoleLogHandler : public Core::LogHandler
    {
    public:
        void log(const std::string & message) override;
        void logError(const std::string & message) override;
        void logInfo(const std::string & message) override;
        void logWarning(const std::string & message) override;
    };

}
