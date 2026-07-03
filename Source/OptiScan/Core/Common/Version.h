#pragma once
#include <cstdint>
#include <optional>
#include <string>

namespace OptiScan::Core::Common
{
    class Version 
    {
    public:

        Version() = default;
        /** Constructor for version without patch */
        Version(uint8_t major, uint8_t minor);
        /** Constructor for version with patch */
        Version(uint8_t major, uint8_t minor, uint8_t patch);
        /** */
        static Version fromString(const std::string & versionString);
        /** */
        uint8_t getMajor() const;
        /** */
        uint8_t getMinor() const;
        /** @throw runtime_error: if version is not patch version */
        uint8_t getPatch() const;
        /** */
        bool isPatchVersion() const;
        /** */
        std::string toString() const;
        /** */
        bool operator == (const Version & other) const;
        /** */
        bool operator < (const Version & other) const;


    private:
        uint8_t _major;
        uint8_t _minor;
        std::optional<uint8_t> _patch = std::nullopt;
    
    };

}
