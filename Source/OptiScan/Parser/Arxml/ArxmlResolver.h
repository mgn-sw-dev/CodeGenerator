#pragma once

#include <OptiScan/Parser/Arxml/ArxmlDatabase.h>
#include <cstdint>
#include <string>
#include <vector>

namespace OptiScan::Parser::Arxml
{
    /** One reference that could not be resolved, or that pointed at an element of a
     *  different type than its DEST attribute announced. */
    struct ArxmlResolveIssue
    {
        /** DEST attribute of the reference. */
        std::string _dest;
        /** Element type actually found at the path, empty when the path is unknown. */
        std::string _elementType;
        /** Path the reference points at. */
        std::string _path;
        /** Path of the element holding the reference. */
        std::string _source;
    };

    struct ArxmlResolveReport
    {
        uint64_t _referenceCount = 0;
        uint64_t _resolvedCount = 0;
        std::vector<ArxmlResolveIssue> _typeMismatches;
        std::vector<ArxmlResolveIssue> _unresolved;
    };

    /** Resolves the path references of a parsed ARXML (plan B4, second phase).
     *
     *  While parsing, a reference can only be kept as the path it was written as,
     *  because the pull parser never moves backwards. Once the file has been read the
     *  database no longer changes, so pointers into its lists are stable and this
     *  class can hand them out.
     *
     *  The DEST attribute of a reference is checked against the element actually found,
     *  so a reference is verified rather than trusted. */
    class ArxmlResolver
    {
    public:
        ArxmlResolver(const ArxmlDatabase & arxmlDatabase);

        /** @return nullptr when the path is unknown or holds a different element type. */
        const Ar::ArCanFrame * findCanFrame(const Ar::ArRef & reference) const;
        /** */
        const Ar::ArCompuMethod * findCompuMethod(const Ar::ArRef & reference) const;
        /** */
        const Ar::ArDataConstr * findDataConstr(const Ar::ArRef & reference) const;
        /** */
        const Ar::ArISignal * findISignal(const Ar::ArRef & reference) const;
        /** */
        const Ar::ArISignalIPdu * findISignalIPdu(const Ar::ArRef & reference) const;
        /** */
        const Ar::ArSwBaseType * findSwBaseType(const Ar::ArRef & reference) const;
        /** */
        const Ar::ArSystemSignal * findSystemSignal(const Ar::ArRef & reference) const;
        /** */
        const Ar::ArUnit * findUnit(const Ar::ArRef & reference) const;
        /** Walks every reference of the model and collects what does not resolve. */
        void validate(ArxmlResolveReport & report) const;

    private:
        const ArxmlDatabase * _arxmlDatabase;

        /** Adds @p reference to @p report, either as resolved or as an issue. */
        void check(const Ar::ArRef & reference, const std::string & source,
            ArxmlResolveReport & report) const;
        /** @return nullptr when the path is unknown or holds a different element type. */
        const ArxmlIdentifiable * find(const Ar::ArRef & reference,
            const std::string & elementType) const;
    };

}
