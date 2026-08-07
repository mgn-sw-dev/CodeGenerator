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

    struct LinAsciiValue
    {
    };

    struct LinBcdValue
    {
    };

    struct LinLogicalValue
    {
        uint32_t _signalValue = 0;
        std::string _textInfo;
    };

    struct LinPhysicalValue
    {
        uint32_t _maxValue = 0;
        uint32_t _minValue = 0;
        double _offset = 0.0;
        double _scale = 0.0;
        std::string _textInfo;
    };

    using LinValue = std::variant<LinAsciiValue, LinBcdValue, LinLogicalValue, LinPhysicalValue>;

    struct LinSignalEncodingType
    {
        std::string _name;
        std::vector<LinValue> _values;
    };

    struct LinSignalRepresentation
    {
        std::string _encodingType;
        std::vector<std::string> _signals;
    };

    struct LinFrameSignal
    {
        std::string _name;
        uint8_t _offset = 0;
        /** */
        LinFrameSignal() = default;
    };

    struct LinFrame
    {
        uint8_t _byteSize = 0;
        uint8_t _id = 0;
        std::string _name;
        std::string _publishedBy;
        std::vector<LinFrameSignal> _signals = {};
        /** */
        LinFrame() = default;
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
        std::vector<LinFrame> _frames;
        std::string _languageVersion;
        MasterNode _masterNode;
        std::string _protocolVersion;
        std::vector<LinSignalEncodingType> _signalEncodingTypes;
        std::vector<LinSignalRepresentation> _signalRepresentations;
        std::vector<LinSignal> _signals;
        double _speed_kBit_per_s;
        std::vector<std::string> _slaveNodes;
    };

}
