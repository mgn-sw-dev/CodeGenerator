#pragma once

#include <OptiScan/Parser/Flags.h>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace OptiScan::Parser::Dbc
{
    class DbcExtendedMultiplexing;
    class DbcMessage;
    class DbcMessageTransmitters;
    class DbcMultiplexorValueRange;
    class DbcSignal;
    class DbcSignalAttribute;
    class DbcSignalBase;
    class DbcSignalEnvVarValueDescriptions;
    class DbcSignalGroup;
    class DbcSignalType;
    class DbcSignalTypeReference;
    class DbcSignalValueDescriptions;
    class DbcValueDescription;
    class DbcValueTable;

    using DbcAttributeValue = std::variant<std::monostate, std::string, uint32_t, int32_t, double>;

    enum class DbcAttributeObjectType
    {
        Unknown,
        EnvVar,
        Network,
        Node,
        Message,
        Signal,
    };

    enum class DbcAttributeValueType
    {
        Unknown,
        Enum,
        Float,
        Hex,
        Int,
        String,
    };

    class DbcAttribute
    {
    public:
        std::string _name;
        DbcAttributeValue _value;
    };

    class DbcAttributeDef
    {
    public:
        std::string _name;
        DbcAttributeObjectType _objectType;
        /** */
        virtual ~DbcAttributeDef() = default;
        /** */
        virtual DbcAttributeValueType valueType() const = 0;
    };

    class DbcAttributeDefEnum : public DbcAttributeDef
    {
    public:
        std::vector<std::string> _values;
        /** */
        virtual DbcAttributeValueType valueType() const override;
    };

    class DbcAttributeDefFloat : public DbcAttributeDef
    {
    public:
        double _maximum;
        double _minimum;
        /** */
        virtual DbcAttributeValueType valueType() const override;
    };

    class DbcAttributeDefHex : public DbcAttributeDef
    {
    public:
        uint32_t _maximum;
        uint32_t _minimum;
        /** */
        virtual DbcAttributeValueType valueType() const override;
    };

    class DbcAttributeDefInt : public DbcAttributeDef
    {
    public:
        int32_t _maximum;
        int32_t _minimum;
        /** */
        virtual DbcAttributeValueType valueType() const override;
    };

    class DbcAttributeDefString : public DbcAttributeDef
    {
    public:
        /** */
        virtual DbcAttributeValueType valueType() const override;
    };

    class DbcAttributeEnvVar : public DbcAttribute
    {
    public:
        std::string _envVarName;
    };

    class DbcAttributeMessage : public DbcAttribute
    {
    public:
        uint32_t _messageId;
    };

    class DbcAttributeNode : public DbcAttribute
    {
    public:
        std::string _nodeName;
    };

    class DbcAttributeSignal : public DbcAttributeMessage
    {
    public:
        std::string _signalName;
    };

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

    class DbcComment
    {
    public:
        std::string _text;
    };

    class DbcCommentEnvVar : public DbcComment
    {
    public:
        std::string _envVarName;
    };

    class DbcCommentMessage : public DbcComment
    {
    public:
        uint32_t _messageId;
    };

    class DbcCommentNode : public DbcComment
    {
    public:
        std::string _nodeName;
    };

    class DbcCommentSignal : public DbcCommentMessage
    {
    public:
        std::string _signalName;
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

    class DbcExtendedMultiplexing
    {
    public:
        uint32_t _messageId;
        std::string _multiplexedSignal;
        std::string _multiplexorSwitch;
        std::vector<DbcMultiplexorValueRange> _multiplexorValueRanges;
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

    enum class DbcMultiplexorIndicator
    {
        None = 0,
        Multiplexor = 1 << 0,
        MultiplexedSignal = 1 << 1,
    };
    using DbcMultiplexorIndicators = Flags<DbcMultiplexorIndicator>;

    class DbcMultiplexorValueRange
    {
    public:
        uint32_t _minimum;
        uint32_t _maximum;
    };

    class DbcSignalBase
    {
    public:
        double _factor = 0.0;
        bool _isBigEndian = false;
        bool _isSigned = false;
        double _maximum = 0.0;
        double _minimum = 0.0;
        double _offset = 0.0;
        uint32_t _precision = 0;
        uint32_t _size_bit = 0;
        std::string _unit;
    protected:
        /** */
        DbcSignalBase() = default;
    };

    class DbcSignal : public DbcSignalBase
    {
    public:
        DbcMultiplexorIndicators _multiplexorIndicators;
        uint32_t _multiplexorSwitchValue = 0;
        std::string _name;
        std::vector<std::string> _receivers;
        uint32_t _startBit = 0;
        /** */
        DbcSignal() = default;
    };

    class DbcSignalEnvVarValueDescriptions
    {
    public:
        std::vector<DbcValueDescription> _descriptions;
        std::string _envVarName;
    };

    class DbcSignalGroup
    {
    public:
        uint32_t _messageId;
        uint32_t _repetitions;
        std::string _name;
        std::vector<std::string> _signalNames;
    };

    class DbcSignalType : public DbcSignalBase
    {
    public:
        double _defaultValue = 0.0;
        std::string _name;
        std::string _valueTable;
        /** */
        DbcSignalType() = default;
    };

    class DbcSignalTypeReference
    {
    public:
        uint32_t _messageId;
        std::string _signalName;
        std::string _signalTypeName;
    };

    class DbcSignalValueDescriptions
    {
    public:
        std::vector<DbcValueDescription> _descriptions;
        uint32_t _messageId;
        std::string _signalName;
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
