#pragma once

#include <OptiScan/Parser/ScanPosition.h>
#include <istream>
#include <vector>

namespace OptiScan::Parser
{

    class CharReader 
    {
    public:
        explicit CharReader(std::istream * input);
        /** */
        const ScanChar & at(size_t index) const;
        /** Check if @c _scanBuffer starts with a specific string pattern. */
        bool bufferStartsWith(const std::string & pattern, size_t offset = 0) const;
        /** */
        bool empty() const;
        /** @return false If end of stream detected.
         *  @throws ios_base::failure. */
        bool fillScanBuffer(size_t count = 1);
        /** */
        const ScanChar & front() const;
        /** */
        static bool isCharDigit(const char & c);
        /** */
        static bool isCharHexDigit(const char & c);
        /** */
        static bool isCharIdentifier(const char & c);
        /** */
        static bool isCharIdentifierStart(const char & c);
        /** Get one char in buffer and return as @c ScanChar type.
         *  Need @ref fillScanBuffer (count) before. */
        ScanChar popBufferFront();
        /** Get count of chars in the buffer a return as string.
         *  Need @ref fillScanBuffer (count) before. */
        std::string popFront(size_t count);
        /** @return ScanPosition. */
        const ScanPosition & streamPosition() const;
        /** */
        bool readIdentifier(ScanPosition & position, std::string & text);
    	/** */
    	bool tryMatchBufferStart(const std::string & pattern, size_t offset = 0);

    private:
        std::vector<ScanChar> _scanBuffer;
        std::istream * _stream;
        ScanPosition _streamPosition;

        /** @return false If end of stream detected.
         *  @throws ios_base::failure. */
        bool readCharFromStream(char & c);
        /** @return false If end of stream detected.
         *  @throws ios_base::failure. */
        bool readCharFromStreamToScanBuffer();
    
    };

}
