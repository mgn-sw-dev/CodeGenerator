
#include <OptiScan/Core/Common/Version.h>
#include <sstream>

using namespace std;

namespace OptiScan::Core::Common
{
	Version::Version(uint8_t major, uint8_t minor)
		: _major(major)
		, _minor(minor)
	{
	}

	Version::Version(uint8_t major, uint8_t minor, uint8_t patch)
		: _major(major)
		, _minor(minor)
		, _patch(patch)
	{
	}

	Version Version::fromString(const string & versionString)
	{
		Version result;
		stringstream ss(versionString);
		uint16_t major = 0, minor = 0, patch = 0;
		char dot1, dot2;

		ss >> major >> dot1 >> minor;
		if (ss >> dot2 >> patch)
		{
			result = Version(static_cast<uint8_t>(major), static_cast<uint8_t>(minor), static_cast<uint8_t>(patch));
		}
		else
		{
			result = Version(static_cast<uint8_t>(major), static_cast<uint8_t>(minor));
		}
		return result;
	}

	uint8_t Version::getMajor() const
	{
		return _major;
	}

	uint8_t Version::getMinor() const
	{
		return _minor;
	}

	uint8_t Version::getPatch() const
	{
		if (!this->isPatchVersion())
		{
			throw runtime_error("Version is not patch version");
		}
		return this->_patch.value();
	}

	bool Version::isPatchVersion() const
	{
		return this->_patch.has_value();
	}

	string Version::toString() const
	{
		string result = to_string(this->_major) + "." + to_string(this->_minor);
		if (this->isPatchVersion())
		{
			result += "." + to_string(this->_patch.value());
		}
		return result;
	}

	bool Version::operator == (const Version & other) const
	{
		return tie(this->_major, this->_minor, this->_patch ) == tie(other._major, other._minor, other._patch);
	}

	bool Version::operator < (const Version & other) const
	{
		return tie(this->_major, this->_minor, this->_patch ) < tie(other._major, other._minor, other._patch);
	}
}
