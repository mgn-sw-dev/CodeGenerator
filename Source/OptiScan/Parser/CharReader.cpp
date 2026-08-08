
#include <OptiScan/Parser/CharReader.h>

using namespace std;

namespace OptiScan::Parser
{
	CharReader::CharReader(istream * input)
		: _scanBuffer()
		, _stream(input)
		, _streamPosition()
	{
	}

	const ScanChar & CharReader::at(size_t index) const
	{
		return this->_scanBuffer.at(index);
	}

	bool CharReader::bufferStartsWith(const std::string & pattern, size_t offset) const
	{
		bool result = offset + pattern.size() <= this->_scanBuffer.size();
		for (size_t i = 0; result && i < pattern.size(); i++)
		{
			result = pattern[i] == this->_scanBuffer[offset + i]._value;
		}
		return result;
	}

	bool CharReader::empty() const
	{
		return this->_scanBuffer.empty();
	}

	bool CharReader::fillScanBuffer(size_t count)
	{
		bool result = true;
		for (size_t i = this->_scanBuffer.size(); result && i < count; i++)
		{
			result = this->readCharFromStreamToScanBuffer();
		}
		return result;
	}

	const ScanChar & CharReader::front() const
	{
		return this->_scanBuffer.front();
	}

	bool CharReader::isCharDigit(const char & c)
	{
		return '0' <= c && c <= '9';
	}

	bool CharReader::isCharHexDigit(const char & c)
	{
		return CharReader::isCharDigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
	}

	bool CharReader::isCharIdentifier(const char & c)
	{
		return CharReader::isCharIdentifierStart(c) || CharReader::isCharDigit(c);
	}

	bool CharReader::isCharIdentifierStart(const char & c)
	{
		return false
			|| ('a' <= c && c <= 'z')
			|| ('A' <= c && c <= 'Z')
			|| c == '_'
		;
	}

	ScanChar CharReader::popBufferFront()
	{
		ScanChar const result = this->_scanBuffer.front();
		this->_scanBuffer.erase(this->_scanBuffer.begin());
		return result;
	}

	std::string CharReader::popFront(size_t count)
	{
		string result;
		result.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			result.push_back(this->popBufferFront()._value);
		}
		return result;
	}

	bool CharReader::readCharFromStream(char & c)
	{
		bool result = false;
		int const value = this->_stream->get();
		if (value == std::char_traits<char>::eof())
		{
			if (this->_stream->bad())
			{
				throw ios_base::failure("Stream read error");
			}
			else
			{
				// normal stream end (eofbit/ failbit set but no I/O error)
			}
		}
		else
		{
			result = true;
			c = static_cast<char>(value);
			if (c == '\n')
			{
				this->_streamPosition._charInLine = 0;
				this->_streamPosition._line++;
			}
			else
			{
				this->_streamPosition._charInLine++;
			}
			this->_streamPosition._char++;
		}
		return result;
	}

	bool CharReader::readCharFromStreamToScanBuffer()
	{
		ScanChar c;
		c._position = this->_streamPosition;
		bool result = this->readCharFromStream(c._value);
		if (result)
		{
			this->_scanBuffer.push_back(c);
		}
		return result;
	}

	bool CharReader::readIdentifier(ScanPosition & position, std::string & text)
	{
		bool result = false;
		if (this->fillScanBuffer())
		{
			const ScanChar & c = this->front();
			if (CharReader::isCharIdentifierStart(c._value))
			{
				result = true;
				position = c._position;
				text.push_back(this->popBufferFront()._value);
				bool checkNext = true;
				while (checkNext)
				{
					checkNext = this->fillScanBuffer();
					if (checkNext)
					{
						const ScanChar & c = this->front();
						checkNext = CharReader::isCharIdentifier(c._value);
						if (checkNext)
						{
							text.push_back(this->popBufferFront()._value);
						}
					}
				}
			}
		}
		return result;
	}

	bool CharReader::tryMatchBufferStart(const string & pattern, size_t offset)
	{
		bool result = this->fillScanBuffer(offset + pattern.size());
		if (result)
		{
			result = this->bufferStartsWith(pattern, offset);
		}
		return result;
	}

	const ScanPosition & CharReader::streamPosition() const
	{
		return this->_streamPosition;
	}


}
