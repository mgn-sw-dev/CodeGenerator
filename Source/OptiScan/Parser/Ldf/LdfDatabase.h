#pragma once

#include <string>
#include <vector>
#include <variant>

namespace OptiScan::Parser::Ldf
{

    struct LinByteArraySignal
    {
        std::vector<uint8_t> _initValueBigEndian = {};
    };

    struct LinScalarSignal
    {
        uint16_t _initValue = 0;
    };

    using LinSignalValue = std::variant<LinByteArraySignal, LinScalarSignal>;

    struct LinSignal
    {
        uint8_t _bitSize = 0;
        std::string _name;
        std::string _publishedBy;
        std::vector<std::string> _subscribedBy;
        LinSignalValue _value;
        /** */
        LinSignal() = default;
    };

    struct MasterNode
    {
        double _jitter_ms = 0.0;
        std::string _name;
        double _timeBase_ms = 0.0;
        /** */
        MasterNode() = default;
    };

    class LdfDatabase 
    {
    public:
        std::string _channelName;
        std::string _languageVersion;
        MasterNode _masterNode;
        std::string _protocolVersion;
        std::vector<LinSignal> _signals;
        double _speed_kBit_per_s;
        std::vector<std::string> _slaveNodes;
    };

}
