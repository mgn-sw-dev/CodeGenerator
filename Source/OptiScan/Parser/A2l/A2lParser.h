#pragma once

#include <OptiScan/Parser/A2l/A2lDatabase.h>
#include <OptiScan/Parser/A2l/A2lTokenReader.h>
#include <fstream>

namespace OptiScan::Parser::A2l
{

    class A2lParser 
    {
    public:
        A2lParser(std::istream * input);
        /** @throws FormatException. */
        void parse(A2lDatabase & a2lDatabase);

    private:
        A2lTokenReader _reader;

        /** @throws FormatException. */
        void parseAsap2Version(McdVersion & version);
        /** @throws FormatException. */
        void parseA2mlVersion(std::optional<McdVersion> & version);
        /** @throws FormatException. */
        void parseBlockBegin(const std::string & keyword);
        /** @throws FormatException. */
        void parseIdent(McdIdent & ident);
        /** @throws FormatException. */
        void parseIdentPart(McdIdentPart & part);
        /** @throws FormatException. */
        void parseProject(McdProject & project);
        /** @throws FormatException. */
        void parseVersion(McdVersion & version);
    
    };

}
