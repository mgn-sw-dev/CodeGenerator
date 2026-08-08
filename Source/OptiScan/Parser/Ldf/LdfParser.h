#pragma once

#include <OptiScan/Parser/Ldf/LdfDatabase.h>
#include <OptiScan/Parser/Ldf/LdfModes.h>
#include <OptiScan/Parser/Ldf/LdfTokenReader.h>
#include <fstream>

namespace OptiScan::Parser::Ldf
{

    class LdfParser 
    {
    public:
        /** */
        LdfParser(std::istream * input);
        /** */
        void getLdfModes(LdfModes & ldfModes) const;
        /** @throws FormatException. */
        void parse(LdfDatabase & ldfDatabase);
        /** */
        void setLdfModes(const LdfModes & ldfModes);
    private:
        LdfModes _ldfModes;
        LdfTokenReader _reader;

        /** @throws FormatException. */
        void parseChannelName(const std::string & languageVersion, std::string & channelName);
        /** @throws FormatException. */
        void parseDescriptionFile();
        /** @throws FormatException. */
        void parseDiagnosticFrames();
        /** @throws FormatException. */
        void parseDiagnosticSignals();
        /** @throws FormatException. */
        void parseEndOfFile() const;
        /** @throws FormatException. */
        void parseEventTriggeredFrames();
        /** @throws FormatException. */
        void parseFrames(std::vector<LinFrame> & frames);
        /** @throws FormatException. */
        void parseFrameSignals(std::vector<LinFrameSignal> & signals);
        /** @throws FormatException. */
        void parseLanguageVersion(std::string & languageVersion);
        /** @throws FormatException. */
        void parseMasterNode(MasterNode & masterNode);
        /** @throws FormatException. */
        void parseNodeAttributeConfigurableFrames(const std::pair<uint8_t, uint8_t> & protocolVersion);
        /** @throws FormatException. */
        void parseNodeAttributeFaultStateSignals();
        /** @throws FormatException. */
        void parseNodeAttributeInitialNad();
        /** @throws FormatException. */
        void parseNodeAttributeNasTimeout();
        /** @throws FormatException. */
        void parseNodeAttributeNcrTimeout();
        /** @throws FormatException. */
        void parseNodeAttributeP2Min();
        /** @throws FormatException. */
        void parseNodeAttributeStMin();
        /** @throws FormatException. */
        void parseNodeAttributes();
        /** @throws FormatException. */
        void parseNodeAttributesForProtocolMajorVersion2(const std::pair<uint8_t, uint8_t> & protocolVersion);
        /** @throws FormatException. */
        void parseNodes(MasterNode & masterNode, std::vector<std::string> & slaveNodes);
        /** @throws FormatException. */
        void parseNodeComposite();
        /** @throws FormatException. */
        void parseProtocolVersion(std::string & protocolVersion);
        /** @throws FormatException. */
        void parseScheduleTableAssignFrameId();
        /** @throws FormatException. */
        void parseScheduleTableAssignFrameIdRange();
        /** @throws FormatException. */
        void parseScheduleTableAssignNad();
        /** @throws FormatException. */
        void parseScheduleTableConditionalChangeNad();
        /** @throws FormatException. */
        void parseScheduleTableDataDump();
        /** @throws FormatException. */
        void parseScheduleTableFreeFormat();
        /** @throws FormatException. */
        void parseScheduleTableSaveConfiguration();
        /** @throws FormatException. */
        void parseScheduleTables(const std::string & languageVersion);
        /** @throws FormatException. */
        void parseSignalEncodingTypeAsciiValue(LinSignalEncodingType & type);
        /** @throws FormatException. */
        void parseSignalEncodingTypeBcdValue(LinSignalEncodingType & type);
        /** @throws FormatException. */
        void parseSignalEncodingTypeLogicalValue(LinSignalEncodingType & type);
        /** @throws FormatException. */
        void parseSignalEncodingTypePhysicalValue(LinSignalEncodingType & type);
        /** @throws FormatException. */
        void parseSignalEncodingTypes(std::vector<LinSignalEncodingType> & signalEncodingTypes);
        /** @throws FormatException. */
        void parseSignalRepresentations(std::vector<LinSignalRepresentation> & signalRepresentations);
        /** @throws FormatException. */
        void parseSignals( std::vector<LinSignal> & signals);
        /** @throws FormatException. */
        void parseSignalInitValues(LinSignal & signal);
        /** @throws FormatException. */
        void parseSporadicFrames();
        /** @throws FormatException. */
        void parseSlaveNodes(std::vector<std::string> & slaveNodes);
        /** @throws FormatException. */
        void parseSpeed(double & speed_kBit_per_s);
        /** @throws FormatException. */
        void parseVersions(std::string & version);

    };

}
