#pragma once

#include <OptiScan/Parser/A2l/A2lTokenReader.h>
#include <OptiScan/Parser/A2l/Mcd/McdIdent.h>
#include <OptiScan/Parser/A2l/Mcd/McdIfData.h>

namespace OptiScan::Parser::A2l::IfData
{
    /** Parser for one interface flavour of an IF_DATA block. Implementations are
     *  registered with A2lParser::addIfDataParser; the core parser itself has no
     *  knowledge of any interface. */
    class IA2lIfDataParser
    {
    public:
        virtual ~IA2lIfDataParser() = default;
        /** True if this parser handles the interface named after the IF_DATA keyword. */
        virtual bool canParse(const Mcd::McdIdent & name) const = 0;
        /** Parses the block content only. The caller has consumed
         *  "/begin IF_DATA <name>" and consumes "/end IF_DATA" afterwards.
         *  @throws FormatException. */
        virtual Mcd::McdIfDataPtr parseContent(A2lTokenReader & reader) = 0;
    };

}
