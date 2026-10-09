#pragma once

namespace OptiScan::Parser::A2l::Mcd
{
    enum class McdByteOrder
    {
        None,
        MsbFirst,
        MsbFirstMswLast,
        MsbLast,
        MsbLastMswFirst,
    };

    enum class McdDeposit
    {
        Unknown,
        Absolute,
        Difference,
    };

}
