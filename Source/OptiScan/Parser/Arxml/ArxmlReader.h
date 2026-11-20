#pragma once

#include <cstdint>
#include <deque>
#include <istream>
#include <string>
#include <vector>

namespace OptiScan::Parser::Arxml
{
    enum class ArxmlNodeKind
    {
        None,
        ElementBegin,
        ElementEnd,
        Text,
    };

    struct ArxmlAttribute
    {
        std::string _name;
        std::string _value;
    };

    struct ArxmlNode
    {
        std::vector<ArxmlAttribute> _attributes;
        ArxmlNodeKind _kind = ArxmlNodeKind::None;
        /** Element name, for ElementBegin and ElementEnd. */
        std::string _name;
        /** Character data, for Text. */
        std::string _text;
        uint64_t _lineNumber = 0;
    };

    /** Pull adapter over Expat.
     *
     *  Expat is a push parser: it calls back into the application. This class turns
     *  that around by letting the callbacks fill a short queue which the caller reads
     *  one node at a time. The input is consumed in fixed size chunks, so memory stays
     *  flat regardless of file size.
     *  Whitespace only character data is dropped. ARXML uses indentation for
     *  readability, and keeping it would produce millions of nodes that every caller
     *  would have to filter out. Character data that contains anything else is passed
     *  through unchanged, including its leading and trailing whitespace. */
    class ArxmlReader
    {
    public:
        ArxmlReader(std::istream * input);
        ~ArxmlReader();
        ArxmlReader(const ArxmlReader &) = delete;
        ArxmlReader & operator = (const ArxmlReader &) = delete;

        /** Value of the attribute named @p name of the current node, or nullptr. */
        const std::string * attribute(const std::string & name) const;
        /** */
        ArxmlNodeKind getKind() const;
        /** Line number of the current node, 1 based. */
        uint64_t getLineNumber() const;
        /** */
        const std::string & getName() const;
        /** */
        const std::string & getText() const;
        /** Advance to the next node.
         *  @return false at the end of the document.
         *  @throws FormatException on malformed input. */
        bool readNext();

    private:
        /** Size of one input chunk handed to Expat. */
        static const size_t BufferSize;

        ArxmlNode _current;
        bool _isFinished;
        std::istream * _input;
        /** Character data is reported in pieces by Expat and collected here until the
         *  next element boundary. */
        std::string _pendingText;
        std::deque<ArxmlNode> _queue;
        /** XML_Parser, kept as void * to keep expat.h out of this header. */
        void * _parser;

        /** Read one chunk and let Expat run over it. @throws FormatException. */
        void feed();
        /** Append the collected character data as a node unless it is whitespace only. */
        void flushPendingText();
        /** @throws FormatException. */
        [[noreturn]] void throwExpatError() const;

        static void onCharacterData(void * userData, const char * text, int length);
        static void onElementBegin(void * userData, const char * name, const char ** attributes);
        static void onElementEnd(void * userData, const char * name);
    };

}
