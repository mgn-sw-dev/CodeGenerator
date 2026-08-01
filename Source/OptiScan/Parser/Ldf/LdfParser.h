#pragma once

#include <OptiScan/Parser/Ldf/LdfDatabase.h>
#include <fstream>

namespace OptiScan::Parser::Ldf
{

    class LdfParser 
    {
    public:
        LdfParser(std::istream * input);
        /** @throws . */
        void parse(LdfDatabase & LdfDatabase);
    };

}
