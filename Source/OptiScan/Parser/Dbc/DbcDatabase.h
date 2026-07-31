#pragma once

#include <string>
#include <variant>
#include <vector>

namespace OptiScan::Parser::Dbc
{
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
        std::vector<std::string> _newSymbols;
        std::string _version;
    };

}
