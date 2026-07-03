
#include <OptiScan/Core/Config/ConfigDatabaseBus.h>

using namespace std;

namespace OptiScan::Core::Config
{
	BusObject::BusObject()
		: _hardwareId(0)
		, _name()
	{
	}

	void BusObject::clear()
	{
		this->_hardwareId = 0;
		this->_name.clear();
	}

	void BusObject::setIdSuffix(const string & suffix)
	{
		if (!this->checkIfIdEndsWith(suffix))
		{
			this->_name = this->_name + suffix;
		}
	}

	bool BusObject::checkIfIdEndsWith(const string & ending) const
	{
		bool result = false;
		if (this->_name.size() >= ending.size())
		{
			result = (0 == this->_name.compare(this->_name.size() - ending.size(), ending.size(), ending));
		}
		return result;
	}
}
