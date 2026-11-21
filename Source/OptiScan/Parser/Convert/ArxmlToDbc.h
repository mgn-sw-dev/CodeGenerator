#pragma once

#include <OptiScan/Parser/Arxml/ArxmlDatabase.h>
#include <OptiScan/Parser/Arxml/ArxmlResolver.h>
#include <OptiScan/Parser/Dbc/DbcDatabase.h>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace OptiScan::Parser::Convert
{
    /** What the conversion had to leave out or had to decide, so a difference against
     *  a DBC can be attributed instead of guessed. */
    struct ArxmlToDbcReport
    {
        /** Signals whose limits were taken over from PHYS-CONSTRS. */
        uint64_t _limitsFromPhys = 0;
        /** Signals whose limits were computed from INTERNAL-CONSTRS with factor and
         *  offset, because no PHYS-CONSTRS were given. */
        uint64_t _limitsFromInternal = 0;
        /** Signals that carry no value range at all. */
        uint64_t _limitsMissing = 0;
        uint64_t _messageCount = 0;
        uint64_t _signalCount = 0;
        /** Mappings pointing at a signal group rather than a single signal. A DBC has
         *  no counterpart for those. */
        uint64_t _skippedGroupMappings = 0;
        /** PDUs of a type other than I-SIGNAL-I-PDU, for example NM-PDU. */
        uint64_t _skippedPdus = 0;
        /** The same, broken down by the DEST attribute of the PDU reference, so the
         *  number can be attributed instead of guessed. */
        std::map<std::string, uint64_t> _skippedPduTypes;
        /** I-SIGNAL-I-PDUs that no CAN frame refers to. Their signals never reach a
         *  message, which is the larger part of the difference to a DBC. */
        uint64_t _unreferencedPdus = 0;
        /** Signals without a COMPU-METHOD; they end up with factor 1 and offset 0. */
        uint64_t _signalsWithoutCompuMethod = 0;
        /** Signals whose conversion is a text table. */
        uint64_t _valueTableCount = 0;
        /** Anything that looked wrong rather than merely absent. */
        std::vector<std::string> _warnings;
    };

    /** Translates a parsed ARXML into the DBC world.
     *
     *  This step is lossy by design: a DBC folds frame and PDU into one message, knows
     *  no signal groups and carries a single value range per signal. Everything that
     *  does not survive is counted in the report rather than dropped silently.
     *
     *  The class sits above Arxml and Dbc so that neither module has to know the
     *  other. */
    class ArxmlToDbc
    {
    public:
        ArxmlToDbc(const Arxml::ArxmlDatabase & arxmlDatabase);
        /** */
        void convert(Dbc::DbcDatabase & dbcDatabase, ArxmlToDbcReport & report) const;

    private:
        const Arxml::ArxmlDatabase * _arxmlDatabase;
        Arxml::ArxmlResolver _resolver;

        /** Factor and offset of a COMPU-METHOD, defaulting to 1 and 0.
         *  @return false when the method carries no rational coefficients. */
        static bool convertCoefficients(const Arxml::Ar::ArCompuMethod & compuMethod,
            double & factor, double & offset, std::vector<std::string> & warnings);
        /** Fills the limits of @p signal and counts where they came from. */
        void convertLimits(const Arxml::Ar::ArDataConstr * dataConstr, double factor, double offset,
            Dbc::DbcSignal & signal, ArxmlToDbcReport & report) const;
        /** Builds the value descriptions of a text table conversion. */
        static void convertValueDescriptions(const Arxml::Ar::ArCompuMethod & compuMethod,
            std::vector<Dbc::DbcValueDescription> & descriptions);
        /** Translates one I-SIGNAL-TO-I-PDU-MAPPING into a DBC signal.
         *  @return false when the mapping has no counterpart in a DBC. */
        bool convertSignal(const Arxml::Ar::ArISignalToIPduMapping & mapping,
            uint32_t pduStartPosition, Dbc::DbcSignal & signal,
            std::vector<Dbc::DbcValueDescription> & descriptions, ArxmlToDbcReport & report) const;
    };

}
