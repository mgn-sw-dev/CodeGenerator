
#include <OptiScan/Core/Config/ConfigDatabaseLin.h>

namespace OptiScan::Core::Config
{
	void LinBusObject::clear()
	{
		this->BusObject::clear();
		this->_ldfName.clear();
	}

}
