#pragma once

#include <OptiScan/Parser/Ldf/LdfDatabase.h>
#include <OptiScan/Parser/Ldf/LdfScanner.h>
#include <OptiScan/Parser/Flags.h>
#include <fstream>

namespace OptiScan::Parser::Ldf
{

    class LdfParser 
    {
#if 0
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
        /** @throws . */
        void parse(LdfDatabase & LdfDatabase);
        /** */
        const LdfScanner & scanner() const;
        /** */
        void setModes(LdfParser::Modes modes);
    private:
        LdfParser::Modes _modes;
        LdfScanner _scanner;
#endif
    };

}
