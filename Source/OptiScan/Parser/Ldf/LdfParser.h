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
        void parseLinDescriptionFile();
        /** @throws FormatException. */
        void parseLinLanguageVersion(std::string & languageVersion);
        /** @throws FormatException. */
        void parseLinProtocolVersion(std::string & protocolVersion);
        /** @throws FormatException. */
        void parseLinVersions(std::string & version);

    };

}
