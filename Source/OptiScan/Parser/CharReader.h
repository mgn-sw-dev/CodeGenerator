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
    	static bool isCharDecimalSeparator(const char & c);
        /** */
        static bool isCharDigit(const char & c);
    	/** */
    	static bool isCharExponentStart(const char & c);
        /** */
        static bool isCharHexDigit(const char & c);
        /** */
        static bool isCharIdentifier(const char & c);
        /** */
        static bool isCharIdentifierStart(const char & c);
        /** Get one char in buffer and return as @c ScanChar type.
         *  Need @ref fillScanBuffer (count) before.
         *  @return ScanChar */
        ScanChar popScanCharFromBufferFront();
        /** Get count of chars in the buffer a return as string.
         *  Need @ref fillScanBuffer (count) before.
         *  @return std::string */
        std::string popStringFromBufferFront(size_t stringSize);
        /** @return ScanPosition. */
        const ScanPosition & streamPosition() const;
        /** */
        bool readIdentifier(ScanPosition & position, std::string & text);
    	/** Try to match single char in buffer. */
    	bool tryMatchBufferChar(const char & c, size_t offset = 0);
    	/** Try to match string in buffer. */
    	bool tryMatchBufferStart(const std::string & pattern, size_t offset = 0);

    private:
    	constexpr static unsigned char Bom[3] = { 0xEF, 0xBB, 0xBF };
        std::vector<ScanChar> _scanBuffer;
        std::istream * _stream;
        ScanPosition _streamPosition;

        /** @return false If end of stream detected.
         *  @throws ios_base::failure. */
        bool readCharFromStream(char & c);
        /** @return false If end of stream detected.
         *  @throws ios_base::failure. */
        bool readCharFromStreamToScanBuffer();
    	/** */
    	void skipUtf8Bom();
    
    };

}
