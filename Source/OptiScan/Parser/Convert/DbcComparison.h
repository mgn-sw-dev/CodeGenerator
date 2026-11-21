#pragma once

#include <OptiScan/Parser/Dbc/DbcDatabase.h>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace OptiScan::Parser::Convert
{
    enum class DbcDifferenceKind
    {
        /** A message of the reference is missing in the candidate. */
        MessageMissing,
        /** The candidate holds a message the reference does not have. */
        MessageAdded,
        MessageAttribute,
        SignalMissing,
        SignalAdded,
        SignalAttribute,
    };

    struct DbcDifference
    {
        /** Name of the attribute that differs, empty for a missing element. */
        std::string _attribute;
        std::string _expected;
        std::string _found;
        DbcDifferenceKind _kind = DbcDifferenceKind::MessageAttribute;
        std::string _messageName;
        std::string _signalName;
    };

    struct DbcComparisonReport
    {
        std::vector<DbcDifference> _differences;
        /** Differences counted per attribute name, for a quick overview. */
        std::map<std::string, uint64_t> _differencesByAttribute;
        uint64_t _messagesMatched = 0;
        uint64_t _messagesOnlyInCandidate = 0;
        uint64_t _messagesOnlyInReference = 0;
        uint64_t _signalsMatched = 0;
        uint64_t _signalsOnlyInCandidate = 0;
        uint64_t _signalsOnlyInReference = 0;
        /** Units that differ as raw bytes but agree once the reference is read as
         *  Latin-1. Not counted as a difference. */
        uint64_t _unitsEqualAfterEncoding = 0;
    };

    /** Compares two DBC models attribute by attribute.
     *
     *  Used to hold a converted ARXML against the DBC that was generated from the same
     *  network by another tool. Neither side is the truth: the reference is itself the
     *  output of a conversion, so a difference says that the two disagree, not who is
     *  right. The report therefore names the attribute and both values instead of
     *  passing a verdict.
     *
     *  Transmitters and receivers are not compared, because they are outside the first
     *  scope of the ARXML parser (plan B6). */
    class DbcComparison
    {
    public:
        DbcComparison() = default;
        /** @param reference: the DBC read from file.
         *  @param candidate: the model converted from the ARXML. */
        void compare(const Dbc::DbcDatabase & reference, const Dbc::DbcDatabase & candidate,
            DbcComparisonReport & report) const;
        /** A DBC written on Windows holds single byte Latin-1, while an ARXML is UTF-8.
         *  Converts the former to the latter so texts can be held against each other. */
        static std::string latin1ToUtf8(const std::string & value);

    private:
        /** Relative comparison, so a factor of 0.00390625 does not fail on the last bit. */
        static bool equalNumbers(double left, double right);
        /** */
        static void addDifference(DbcDifference && difference, DbcComparisonReport & report);
        /** */
        void compareMessage(const Dbc::DbcMessage & reference, const Dbc::DbcMessage & candidate,
            DbcComparisonReport & report) const;
        /** */
        void compareSignal(const std::string & messageName, const Dbc::DbcSignal & reference,
            const Dbc::DbcSignal & candidate, DbcComparisonReport & report) const;
    };

}
