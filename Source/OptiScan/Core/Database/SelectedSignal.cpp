
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
		this->_a2lName.clear();
		this->_busId.clear();
		this->_dbcName.clear();
		this->_displayName.clear();
		this->_frameName.clear();
		this->_frameId.clear();
		this->_hardwareId = 0;
		this->_ldfName.clear();
		this->_messageId.clear();
		this->_messageName.clear();
		this->_nodeName.clear();
		this->_sampleFrequency_Hz = 0.0;
		this->_signalName.clear();
		this->_signalType = SelectedSignalType::Unknown;
		this->_xcpPlus = false;
	}
}
