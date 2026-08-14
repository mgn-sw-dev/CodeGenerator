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

        /** */
        void parseAsap2Version(McdVersion & version);
        /** */
        void parseA2mlVersion(McdVersion & version);
        /** */
        void parseVersion(McdVersion & version);
    
    };

}
