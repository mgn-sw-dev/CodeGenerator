#pragma once

#include <OptiScan/Parser/Ldf/LinSignal.h>
#include <OptiScan/Parser/Ldf/LinSignalEncodingType.h>
#include <string>
#include <vector>
#include <variant>

namespace OptiScan::Parser::Ldf
{

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
