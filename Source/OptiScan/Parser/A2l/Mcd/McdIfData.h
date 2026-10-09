#pragma once

#include <OptiScan/Parser/A2l/Mcd/McdIdent.h>
#include <memory>

namespace OptiScan::Parser::A2l::Mcd
{
    /** Base of all IF_DATA contents. Concrete types are provided by the interface
     *  specific parsers, for example XCP. */
    class McdIfData
    {
    public:
        virtual ~McdIfData() = default;
        /** Name of the interface as written after the IF_DATA keyword. */
        virtual const McdIdent & name() const = 0;
    };

    using McdIfDataPtr = std::unique_ptr<McdIfData>;

}
