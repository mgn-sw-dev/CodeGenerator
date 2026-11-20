#pragma once

#include <OptiScan/Parser/Arxml/Ar/ArCanCluster.h>
#include <OptiScan/Parser/Arxml/Ar/ArCanFrame.h>
#include <OptiScan/Parser/Arxml/Ar/ArCanFrameTriggering.h>
#include <OptiScan/Parser/Arxml/Ar/ArCompuMethod.h>
#include <OptiScan/Parser/Arxml/Ar/ArDataConstr.h>
#include <OptiScan/Parser/Arxml/Ar/ArISignal.h>
#include <OptiScan/Parser/Arxml/Ar/ArISignalIPdu.h>
#include <OptiScan/Parser/Arxml/Ar/ArSwBaseType.h>
#include <OptiScan/Parser/Arxml/Ar/ArSystemSignal.h>
#include <OptiScan/Parser/Arxml/Ar/ArUnit.h>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace OptiScan::Parser::Arxml
{
    /** One identifiable element of the ARXML, that is one element carrying a
     *  SHORT-NAME. Its path is the key under which it is held in the database. */
    struct ArxmlIdentifiable
    {
        /** Value of _elementIndex for an element that is indexed but not modelled. */
        static inline const size_t NoIndex = static_cast<size_t>(-1);

        /** Position inside the list of ArxmlDatabase that matches _elementType, or
         *  NoIndex. An index is used instead of a pointer because the lists grow
         *  while parsing and a pointer would not survive that. */
        size_t _elementIndex = ArxmlIdentifiable::NoIndex;
        /** Name of the XML element, for example "I-SIGNAL". */
        std::string _elementType;
        uint64_t _lineNumber = 0;
    };

    /** Content of one ARXML file.
     *
     *  The path index is the backbone of the two phase approach (plan B4): the parser
     *  cannot follow a reference while reading, because the pull parser only moves
     *  forward. References are therefore kept as paths and resolved against this index
     *  once the file has been read. */
    class ArxmlDatabase
    {
    public:
        std::vector<Ar::ArCanCluster> _canClusters;
        std::vector<Ar::ArCanFrame> _canFrames;
        std::vector<Ar::ArCanFrameTriggering> _canFrameTriggerings;
        std::vector<Ar::ArCompuMethod> _compuMethods;
        std::vector<Ar::ArDataConstr> _dataConstrs;
        /** Paths that occur more than once. AUTOSAR requires them to be unique, so a
         *  non empty list points at a broken file or at a gap in the parser. */
        std::vector<std::string> _duplicatePaths;
        std::vector<Ar::ArISignal> _iSignals;
        std::vector<Ar::ArISignalIPdu> _iSignalIPdus;
        /** AUTOSAR path, for example "/Signals/BACKBONE/EngSpd", to element. */
        std::map<std::string, ArxmlIdentifiable> _pathIndex;
        std::vector<Ar::ArSwBaseType> _swBaseTypes;
        std::vector<Ar::ArSystemSignal> _systemSignals;
        std::vector<Ar::ArUnit> _units;
    };

}
