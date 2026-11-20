#pragma once

#include <OptiScan/Parser/Arxml/ArxmlReader.h>
#include <cstdint>
#include <istream>
#include <string>

namespace OptiScan::Parser::Arxml
{
    /** Navigation over the node stream of @ref ArxmlReader, plus typed values.
     *
     *  Knows about elements and values, not about AUTOSAR. Because the underlying reader is a pull
     *  parser the stream only moves forward - there is no way back to an element that
     *  has already been passed. */
    class ArxmlTokenReader
    {
    public:
        ArxmlTokenReader(std::istream * input);

        /** Value of the attribute named @p name of the current node, or nullptr. */
        const std::string * attribute(const std::string & name) const;
        /** Attribute value, or an empty string when the attribute is absent. */
        const std::string & attributeOrEmpty(const std::string & name) const;
        /** */
        uint64_t depth() const;
        /** */
        bool hasNode() const;
        /** */
        ArxmlNodeKind kind() const;
        /** */
        uint64_t line() const;
        /** @throws FormatException. */
        void matchElementBegin(const std::string & name) const;
        /** @throws FormatException. */
        void matchElementEnd(const std::string & name) const;
        /** @throws FormatException. */
        void matchKind(ArxmlNodeKind kind) const;
        /** */
        const std::string & name() const;
        /** Expect "<name>" and consume it. @throws FormatException. */
        void parseElementBegin(const std::string & name);
        /** Expect "</name>" and consume it. @throws FormatException. */
        void parseElementEnd(const std::string & name);
        /** At "<name>": read the character data and consume up to and including
         *  "</name>". An element without content yields an empty string.
         *  @throws FormatException. */
        void parseElementText(const std::string & name, std::string & value);
        /** Content of the current element as a floating point number.
         *  @throws FormatException. */
        void parseElementDouble(const std::string & name, double & value);
        /** @throws FormatException. */
        void parseElementInt64(const std::string & name, int64_t & value);
        /** @throws FormatException. */
        void parseElementUInt16(const std::string & name, uint16_t & value);
        /** @throws FormatException. */
        void parseElementUInt32(const std::string & name, uint32_t & value);
        /** @throws FormatException. */
        void parseElementUInt64(const std::string & name, uint64_t & value);
        /** Advance to the next node.
         *  @return false at the end of the document.
         *  @throws FormatException. */
        bool readNext();
        /** At an ElementBegin: consume the element including all of its children.
         *  @throws FormatException. */
        void skipElement();
        /** */
        const std::string & text() const;
        /** Build a message with the current line and throw a FormatException.
         *  @param context: "Class::method" of the caller.
         *  @throws FormatException. */
        [[noreturn]] void throwFormatError(const std::string & context, const std::string & message) const;
        /** Human readable name of @p kind, used in error messages. */
        static std::string nodeKindToText(ArxmlNodeKind kind);
        /** */
        bool tryMatchElementBegin(const std::string & name) const;
        /** */
        bool tryMatchElementEnd(const std::string & name) const;

    private:
        uint64_t _depth;
        bool _hasNode;
        ArxmlReader _reader;

        /** @throws FormatException. */
        void parseElementTextChecked(const std::string & name, const std::string & context, std::string & value);
    };

}
