#pragma once

#include <OptiScan/Parser/Dbc/DbcAttribute.h>
#include <OptiScan/Parser/Dbc/DbcBitTiming.h>
#include <OptiScan/Parser/Dbc/DbcComment.h>
#include <OptiScan/Parser/Dbc/DbcMessage.h>
#include <string>
#include <vector>

namespace OptiScan::Parser::Dbc
{
    class DbcDatabase
    {
    public:
        std::vector<DbcAttribute> _attributes;
        std::vector<DbcAttribute> _attributeDefaults;
        std::vector<DbcAttributeDef> _attributeDefs;
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

}
