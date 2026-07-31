#pragma once

#include <OptiScan/Parser/Dbc/DbcAttribute.h>
#include <OptiScan/Parser/Dbc/DbcComment.h>
#include <OptiScan/Parser/Dbc/DbcSignal.h>
#include <memory>
#include <string>
#include <vector>

namespace OptiScan::Parser::Dbc
{
    class DbcMessage;
    class DbcMessageTransmitters;
    class DbcSignal;
    class DbcSignalAttribute;
    class DbcSignalBase;
    class DbcSignalType;
    class DbcValueDescription;
    class DbcValueTable;

    class DbcBitTiming
    {
    public:
        uint32_t _baudrate = 0;
        uint32_t _btr1 = 0;
        uint32_t _btr2 = 0;
        /** */
        DbcBitTiming() = default;
        /** */
        bool hasNonZero() const;
    };

    class DbcDatabase
    {
    public:
        std::vector<DbcAttribute> _attributes;
        std::vector<DbcAttribute> _attributeDefaults;
        std::vector<std::shared_ptr<DbcAttributeDef>> _attributeDefs;
        std::vector<DbcAttributeEnvVar> _attributeEnvVars;
        std::vector<DbcAttributeMessage> _attributeMessages;
        std::vector<DbcAttributeNode> _attributeNodes;
        std::vector<DbcAttributeSignal> _attributeSignals;
        DbcBitTiming _bitTiming;
        std::vector<DbcComment> _comments;
        std::vector<DbcCommentEnvVar> _commentEnvVars;
        std::vector<DbcCommentMessage> _commentMessages;
        std::vector<DbcCommentNode> _commentNodes;
        std::vector<DbcCommentSignal> _commentSignals;
        std::vector<DbcExtendedMultiplexing> _extendedMultiplexing;
        std::vector<DbcMessage> _messages;
        std::vector<DbcMessageTransmitters> _messageTransmitters;
        std::vector<std::string> _newSymbols;
        std::vector<std::string> _nodes;
        std::vector<DbcSignalGroup> _signalGroups;
        std::vector<DbcSignalType> _signalTypes;
        std::vector<DbcSignalTypeReference> _signalTypeReferences;
        std::vector<DbcSignalValueDescriptions> _signalValueDescriptions;
        std::vector<DbcSignalEnvVarValueDescriptions> _signalEnvVarValueDescriptions;
        std::vector<DbcValueTable> _valueTables;
        std::string _version;
    };

    class DbcMessage
    {
    public:
        static constexpr uint32_t ExtendedCanIdMask = 0x7FFF'FFFF;
        uint32_t _id;
        std::string _name;
        std::vector<DbcSignal> _signals;
        uint32_t _size_byte;
        std::string _transmitter;
        /** */
        DbcMessage();
        /** */
        uint32_t extendedCanId() const;
        /** */
        bool isExtendedCanId() const;
        /** */
        void setExtendedCanId(uint32_t id);
    };

    class DbcMessageTransmitters
    {
    public:
        uint32_t _messageId;
        std::vector<std::string> _transmitters;
    };

    class DbcValueDescription
    {
    public:
        std::string _description;
        double _value = 0.0;
        /** */
        DbcValueDescription() = default;
    };

    class DbcValueTable
    {
    public:
        std::vector<DbcValueDescription> _valueDescriptions;
        std::string _name;
    };

}
