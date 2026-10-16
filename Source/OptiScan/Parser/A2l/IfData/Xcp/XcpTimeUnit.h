#pragma once

#include <cstdint>

namespace OptiScan::Parser::A2l::IfData::Xcp
{
    /** Time unit of a DAQ event channel, as a value object around the ASAP2 unit id.
     *  Together with the cycle count it yields the event period. */
    class XcpTimeUnit
    {
    public:
        static const XcpTimeUnit _1_ns;
        static const XcpTimeUnit _10_ns;
        static const XcpTimeUnit _100_ns;
        static const XcpTimeUnit _1_us;
        static const XcpTimeUnit _10_us;
        static const XcpTimeUnit _100_us;
        static const XcpTimeUnit _1_ms;
        static const XcpTimeUnit _10_ms;
        static const XcpTimeUnit _100_ms;
        static const XcpTimeUnit _1_s;
        /** */
        explicit XcpTimeUnit(uint8_t id);
        /** */
        uint8_t id() const;
        /** Period in milliseconds for @p cycle units. NaN for an unknown unit id. */
        double to_ms(uint8_t cycle) const;

    private:
        uint8_t _id;
    };

}
