#pragma once

#include <OptiScan/Parser/Arxml/ArxmlDatabase.h>
#include <OptiScan/Parser/Arxml/ArxmlTokenReader.h>
#include <cstdint>
#include <istream>
#include <optional>
#include <string>

namespace OptiScan::Parser::Arxml
{

    class ArxmlParser
    {
    public:
        ArxmlParser(std::istream * input);
        /** @throws FormatException. */
        void parse(ArxmlDatabase & arxmlDatabase);

    private:
        struct ArxmlElementContext
        {
            ArxmlDatabase & _arxmlDatabase;
            /** Path of the element, or of the enclosing identifiable if the element
             *  has no SHORT-NAME. */
            std::string _path;
            /** Empty if the element has no SHORT-NAME. */
            std::string _shortName;
            /** Position in the matching database vector, NoIndex if not modelled. */
            size_t _elementIndex;
        };

        ArxmlTokenReader _reader;

        /** Children of an element that is not modelled: recurse so nested identifiable
         *  elements still reach the index. Stops on the closing node of @p elementType.
         *  @throws FormatException. */
        void parseChildren(ArxmlDatabase & arxmlDatabase, const std::string & path,
            const std::string & elementType);
        /** @throws FormatException. */
        void parseCanCluster(ArxmlElementContext & context);
        /** @throws FormatException. */
        void parseCanFrame(ArxmlElementContext & context);
        /** @throws FormatException. */
        void parseCanFrameTriggering(ArxmlElementContext & context);
        /** @throws FormatException. */
        void parseCompuMethod(ArxmlElementContext & context);
        /** @throws FormatException. */
        void parseCompuScale(Ar::ArCompuMethod & compuMethod);
        /** Reads a list of V elements into @p values. @throws FormatException. */
        void parseCoefficients(const std::string & elementType, std::vector<double> & values);
        /** @throws FormatException. */
        void parseDataConstr(ArxmlElementContext & context);
        /** */
        void parseDescription(std::optional<Ar::ArDescription> & description);
        /** Walk one element and everything below it.
         *
         *  An element that carries a SHORT-NAME as its first child is identifiable in
         *  the AUTOSAR sense: its path is the path of the enclosing identifiable plus
         *  its own short name, and it is registered in the index. Everything else only
         *  passes the path through, so grouping elements such as AR-PACKAGES or
         *  ELEMENTS do not show up in paths.
         *
         *  @param parentPath: path of the enclosing identifiable, empty at the root.
         *  @throws FormatException. */
        void parseElement(ArxmlDatabase & arxmlDatabase, const std::string & parentPath);
        /** @throws FormatException. */
        void parseISignal(ArxmlElementContext & context);
        /** @throws FormatException. */
        void parseISignalIPdu(ArxmlElementContext & context);
        /** Reads one I-SIGNAL-TO-I-PDU-MAPPING and registers its path. The mapping is
         *  held inside the PDU, so no element index is recorded for it.
         *  @throws FormatException. */
        void parseISignalToIPduMapping(const ArxmlElementContext & context,
            Ar::ArISignalIPdu & iSignalIPdu);
        /** Descends through I-PDU-TIMING-SPECIFICATIONS and picks up the cyclic timing.
         *  @throws FormatException. */
        void parseIPduTiming(const std::string & elementType, Ar::ArISignalIPdu & iSignalIPdu);
        /** Reads one PDU-TO-FRAME-MAPPING and registers its path.
         *  @throws FormatException. */
        void parsePduToFrameMapping(const ArxmlElementContext & context,
            Ar::ArCanFrame & canFrame);
        /** LOWER-LIMIT and UPPER-LIMIT, which may carry INF or -INF instead of a number.
         *  @throws FormatException. */
        void parseLimit(const std::string & elementType, std::optional<double> & value);
        /** A wrapper such as INIT-VALUE holding a NUMERICAL-VALUE-SPECIFICATION.
         *  @throws FormatException. */
        void parseNumericalValue(const std::string & elementType, std::optional<double> & value);
        /** An element whose text is a path and which carries a DEST attribute.
         *  @throws FormatException. */
        void parseRef(const std::string & elementType, Ar::ArRef & reference);
        /** A wrapper holding a single VALUE element, such as TIME-PERIOD.
         *  @throws FormatException. */
        void parseValueWrapper(const std::string & elementType, std::optional<double> & value);
        /** @throws FormatException. */
        void parseSwBaseType(ArxmlElementContext & context);
        /** The SW-DATA-DEF-PROPS-CONDITIONAL below PHYSICAL-PROPS of a SYSTEM-SIGNAL.
         *  @throws FormatException. */
        void parseSystemSignalProps(Ar::ArSystemSignal & systemSignal);
        /** The SW-DATA-DEF-PROPS-CONDITIONAL below NETWORK-REPRESENTATION-PROPS of an
         *  I-SIGNAL. @throws FormatException. */
        void parseISignalProps(Ar::ArISignal & iSignal);
        /** @throws FormatException. */
        void parseSystemSignal(ArxmlElementContext & context);
        /** @throws FormatException. */
        void parseUnit(ArxmlElementContext & context);
        /** @throws FormatException. */
        void registerPath(ArxmlDatabase & arxmlDatabase, const std::string & path,
            const std::string & elementType, uint64_t lineNumber, size_t elementIndex);
    };

}
