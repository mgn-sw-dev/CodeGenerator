#pragma once

#include <OptiScan/Parser/Flags.h>
#include <string>
#include <variant>
#include <vector>

namespace OptiScan::Parser::Dbc
{

    class DbcMessage;
    class DbcSignal;
    class DbcSignalAttribute;
    class DbcSignalBase;

    using DbcAttributeValue = std::variant<std::monostate, std::string, uint32_t, int32_t, double>;

    class DbcAttribute
    {
    public:
        std::string _name;
        DbcAttributeValue _value;
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

    class DbcDatabase
    {
    public:
        std::vector<DbcAttribute> _attributes;
        std::vector<DbcAttributeEnvVar> _attributeEnvVars;
        std::vector<DbcAttributeMessage> _attributeMessages;
        std::vector<DbcAttributeNode> _attributeNodes;
        std::vector<DbcAttributeSignal> _attributeSignals;
        DbcBitTiming _bitTiming;
        std::vector<DbcMessage> _messages;
        std::vector<std::string> _newSymbols;
        std::vector<std::string> _nodes;
        std::string _version;
    };

    class DbcMessage
    {
    public:
        static uint32_t const ExtendedCanIdMask = 0x7FFF'FFFF;
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

    enum class DbcMultiplexorIndicator
    {
        None = 0,
        Multiplexor = 1 << 0,
        MultiplexedSignal = 1 << 1,
    };
    using DbcMultiplexorIndicators = Flags<DbcMultiplexorIndicator>;

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

}
