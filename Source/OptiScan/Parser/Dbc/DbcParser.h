#pragma once

#include <OptiScan/Parser/Dbc/DbcDatabase.h>
#include <OptiScan/Parser/Dbc/DbcTokenReader.h>
#include <fstream>
#include <vector>

namespace OptiScan::Parser::Dbc
{

    class DbcParserObserver
    {
    public:
        /** @throws DbcFormatException. */
        virtual void onUnknownKeyword(const DbcToken & token);
    protected:
        /** */
        virtual ~DbcParserObserver();
    };

    class DbcParser 
    {
    public:
        DbcParser(std::istream * input);
        /** */
        DbcParserObserver * observer() const;
        /** @throws DbcFormatException. */
        void parse(DbcDatabase & dbcDatabase);
        /** */
        void setObserver(DbcParserObserver * observer);
    private:
        DbcTokenReader _reader;
        DbcParserObserver * _observer;

        /** @throws DbcFormatException. */
        void parseAttribute(DbcDatabase & dbcDatabase);
        /** @throws DbcFormatException. */
        void parseAttributeDef(std::vector<DbcAttributeDef> & attributeDefs);
        /** @throws DbcFormatException. */
        void parseAttributeDefault(std::vector<DbcAttribute> & attributeDefaults);
        /** @throws DbcFormatException. */
        void parseAttributeEnvVar(const std::string & name, std::vector<DbcAttributeEnvVar> & attributeEnvVars);
        /** @throws DbcFormatException. */
        void parseAttributeGlobal(const std::string & name, std::vector<DbcAttribute> & attributes);
        /** @throws DbcFormatException. */
        void parseAttributeMessage(const std::string & name, std::vector<DbcAttributeMessage> & attributeMessages);
        /** @throws DbcFormatException. */
        void parseAttributeNode(const std::string & name, std::vector<DbcAttributeNode> & attributeNodes);
        /** @throws DbcFormatException. */
        void parseAttributeSignal(const std::string & name, std::vector<DbcAttributeSignal> & attributeSignals);
        /** @throws DbcFormatException. */
        void parseAttributeValue(DbcAttributeValue & value);
        /** @throws DbcFormatException. */
        void parseBitTiming(DbcBitTiming & bitTiming);
        /** @throws DbcFormatException. */
        void parseComment(DbcDatabase & dbcDatabase);
        /** @throws DbcFormatException. */
        void parseCommentEnvVar(std::vector<DbcCommentEnvVar> & commentEnvVars);
        /** @throws DbcFormatException. */
        void parseCommentGlobal(std::vector<DbcComment> & comments);
        /** @throws DbcFormatException. */
        void parseCommentMessage(std::vector<DbcCommentMessage> & commentMessages);
        /** @throws DbcFormatException. */
        void parseCommentNode(std::vector<DbcCommentNode> & commentNodes);
        /** @throws DbcFormatException. */
        void parseCommentSignal(std::vector<DbcCommentSignal> & commentSignals);
        /** @throws DbcFormatException. */
        void parseEndOfLine();
        /** @throws DbcFormatException. */
        void parseExtendedMultiplexing(std::vector<DbcExtendedMultiplexing> & extendedMultiplexing);
        /** @throws DbcFormatException. */
        void parseMessage(std::vector<DbcMessage> & messages);
        /** @throws DbcFormatException. */
        void parseMessageTransmitters(std::vector<DbcMessageTransmitters> & transmitters);
        /** @throws DbcFormatException. */
        void parseNewSymbols(std::vector<std::string> & symbols);
        /** @throws DbcFormatException. */
        void parseNodes(std::vector<std::string> & nodes);
        /** @throws DbcFormatException. */
        void parseSignal(std::vector<DbcSignal> & signalList);
        /** @throws DbcFormatException. */
        void parseSignalBase(DbcSignalBase & item);
        /** @throws DbcFormatException. */
        void parseSignalGroup(std::vector<DbcSignalGroup> & groups);
        /** @throws DbcFormatException. */
        void parseSignalTypeOrReference(DbcDatabase & dbcDatabase);
        /** @throws DbcFormatException. */
        void parseTailOfUnknownKeywordLine(const DbcToken & token);
        /** @throws DbcFormatException. */
        void parseValueDescription(std::vector<DbcValueDescription> & descriptions);
        /** @throws DbcFormatException. */
        void parseValueDescriptions(DbcDatabase & dbcDatabase);
        /** @throws DbcFormatException. */
        void parseValueTable(std::vector<DbcValueTable> & tables);
        /** @throws DbcFormatException. */
        void parseVersion(std::string & version);
    };

}
