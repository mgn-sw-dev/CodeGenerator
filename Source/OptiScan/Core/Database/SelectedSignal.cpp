
#include <OptiScan/Core/Database/SelectedSignal.h>

namespace OptiScan::Core::Database
{
	SelectedSignal::SelectedSignal()
		: _a2lName()
		, _busId()
		, _dbcName()
		, _displayName()
		, _frameName()
		, _frameId()
		, _hardwareId(0)
		, _ldfName()
		, _messageId()
		, _messageName()
		, _nodeName()
		, _sampleFrequency_Hz(0.0)
		, _signalName()
		, _signalType(SelectedSignalType::Unknown)
		, _xcpPlus(false)
	{
	}

	void SelectedSignal::clear()
	{
		*this = SelectedSignal();
	}
}
