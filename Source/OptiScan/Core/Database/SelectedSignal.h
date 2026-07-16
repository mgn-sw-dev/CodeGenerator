#pragma once

#include <string>

namespace OptiScan::Core::Database
{

    enum class SelectedSignalType : uint8_t
    {
        Unknown = 0,
        Can     = 1,
        Lin     = 2,
        Xcp     = 3,
    };

    class SelectedSignal
    {
    public:
        std::string _a2lName;
        std::string _busId;
        std::string _dbcName;
        std::string _displayName;
        std::string _frameName;
        std::string _frameId;
        uint8_t _hardwareId;
        std::string _ldfName;
        std::string _messageId;
        std::string _messageName;
        std::string _nodeName;
        double _sampleFrequency_Hz;
        std::string _signalName;
        SelectedSignalType _signalType;
        bool _xcpPlus;

        SelectedSignal();
        /** */
        void clear();
    };

}
