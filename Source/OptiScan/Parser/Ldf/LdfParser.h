#pragma once

#include <OptiScan/Parser/Ldf/LdfDatabase.h>
#include <OptiScan/Parser/Ldf/LdfTokenReader.h>
#include <OptiScan/Parser/Flags.h>
#include <fstream>

namespace OptiScan::Parser::Ldf
{

    class LdfParser 
    {
    public:
        enum class Mode : int
        {
            None = 0,
            AllowSignalEncodingType32 = 1 << 0,
            AllowSignalWithoutSubscriber = 1 << 1,
        };
        using Modes = Flags<Mode>;
        LdfParser(std::istream * input);
        /** */
        LdfParser::Modes modes() const;
        /** @throws FormatException. */
        void parse(LdfDatabase & ldfDatabase);
        /** */
        void setModes(LdfParser::Modes modes);
    private:
        LdfParser::Modes _modes;
        LdfTokenReader _reader;

        /** @throws FormatException. */
        void parseChannelName(const std::string & languageVersion, std::string & channelName);
        /** @throws FormatException. */
        void parseDescriptionFile();
        /** @throws FormatException. */
        void parseLanguageVersion(std::string & languageVersion);
        /** @throws FormatException. */
        void parseMasterNode(MasterNode & masterNode);
        /** @throws FormatException. */
        void parseNodes(MasterNode & masterNode, std::vector<std::string> & slaveNodes);
        /** @throws FormatException. */
        void parseNodeComposite();
        /** @throws FormatException. */
        void parseProtocolVersion(std::string & protocolVersion);
        /** @throws FormatException. */
        void parseSignals( std::vector<LinSignal> & _signals);
        /** @throws FormatException. */
        void parseSlaveNodes(std::vector<std::string> & slaveNodes);
        /** @throws FormatException. */
        void parseSpeed(double & speed_kBit_per_s);
        /** @throws FormatException. */
        void parseVersions(std::string & version);

    };

}
