
#include <OptiScan/Core/Database/SystemMapping.h>

namespace OptiScan::Core::Database
{

	SystemMapping::SystemMapping()
		: _canMappings(SystemMapping::MaxCans)
		, _linMappings(SystemMapping::MaxLins)
	{
		this->clear();
	}

	void SystemMapping::clear()
	{
		this->_canMappings.assign(SystemMapping::MaxCans, MappingCanObject());
		this->_linMappings.assign(SystemMapping::MaxLins, MappingLinObject());
	}

	bool SystemMapping::getCanMapping(uint8_t hardwareId, MappingCanObject & mapping) const
	{
		bool result = false;
		if (hardwareId >= 1 && hardwareId <= SystemMapping::MaxCans)
		{
			mapping = this->_canMappings[hardwareId-1];
			if (!mapping._busName.empty())
			{
				result = true;
			}
		}
		return result;
	}

	bool SystemMapping::getLinMapping(uint8_t hardwareId, MappingLinObject & mapping) const
	{
		bool result = false;
		 if (hardwareId >= 1 && hardwareId <= SystemMapping::MaxLins)
		 {
			 mapping = this->_linMappings[hardwareId-1];
			 if (!mapping._busName.empty())
			 {
				 result = true;
			 }
		 }
		return result;
	}

	void SystemMapping::setCanMapping(uint8_t hardwareId, const MappingCanObject & mapping)
	{
		if (hardwareId >= 1 && hardwareId <= SystemMapping::MaxCans)
		{
			this->_canMappings[hardwareId-1] = mapping;
		}
	}

	void SystemMapping::setLinMapping(uint8_t hardwareId, const MappingLinObject & mapping)
	{
		if (hardwareId >= 1 && hardwareId <= SystemMapping::MaxLins)
		{
			this->_linMappings[hardwareId-1] = mapping;
		}
	}
}
