
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpIfData.h>

using namespace OptiScan::Parser::A2l::Mcd;
using namespace std;

namespace OptiScan::Parser::A2l::IfData::Xcp
{
	uint32_t XcpCanId::extendedId() const
	{
		return this->_value & XcpCanId::ExtendedMask;
	}

	bool XcpCanId::isExtended() const
	{
		return 0 != (this->_value & ~XcpCanId::ExtendedMask);
	}

	XcpCommunicationModeBlock::XcpCommunicationModeBlock()
		: _masterIsSupported(false)
		, _masterMaxBs(0)
		, _masterMaxSt(0)
		, _slaveIsSupported(false)
	{
	}

	XcpCommunicationModeInterleaved::XcpCommunicationModeInterleaved()
		: _queueSize(0)
	{
	}

	XcpEvent::XcpEvent()
		: _canDaq(false)
		, _canStim(false)
		, _eventChannelName()
		, _eventChannelNumber(0)
		, _eventChannelShortName()
		, _maxDaqList(0)
		, _priority(0)
		, _timeCycle(0)
		, _timeUnit(XcpTimeUnit::_1_ms)
	{
	}

	XcpDaq::XcpDaq()
		: _addressExtension(XcpAddressExtension::Free)
		, _events()
		, _granularityOdtEntrySizeDaq(XcpGranularityOdtEntrySizeDaq::Byte)
		, _identificationField(XcpIdentificationFieldType::Absolute)
		, _isDynamic(false)
		, _isPidOffSupported(false)
		, _isPrescalerSupported(false)
		, _isResumeSupported(false)
		, _maxDaq(0)
		, _maxEventChannel(0)
		, _maxOdtEntrySizeDaq(0)
		, _minDaq(0)
		, _optimizationType(XcpOptimizationType::Default)
		, _overloadIndication(XcpOverloadIndication::None)
	{
	}

	XcpDaqListCanId::XcpDaqListCanId()
		: _daqListNumber(0)
		, _fixedTo()
		, _isVariable(false)
	{
	}

	XcpPag::XcpPag()
		: _isFreezeSupported(false)
		, _maxSegments(0)
	{
	}

	XcpProtocolLayer::XcpProtocolLayer()
		: _addressGranularity(XcpAddressGranularity::Byte)
		, _byteOrder(XcpByteOrder::MsbLast)
		, _communicationModeSupported()
		, _maxCto(0)
		, _maxDto(0)
		, _optionalCommands()
		, _seedAndKeyExternalFunction()
		, _t1_ms(0)
		, _t2_ms(0)
		, _t3_ms(0)
		, _t4_ms(0)
		, _t5_ms(0)
		, _t6_ms(0)
		, _t7_ms(0)
		, _version(0)
	{
	}

	XcpCommonParameters::XcpCommonParameters()
		: _daq()
		, _pag()
		, _protocolLayer()
	{
	}

	XcpCommonParameters XcpCommonParameters::overruled(const XcpCommonParameters & source) const
	{
		XcpCommonParameters result = *this;
		if (source._daq.has_value())
		{
			result._daq = source._daq;
		}
		if (source._pag.has_value())
		{
			result._pag = source._pag;
		}
		if (source._protocolLayer.has_value())
		{
			result._protocolLayer = source._protocolLayer;
		}
		return result;
	}

	XcpCanParameters::XcpCanParameters()
		: _baudrate_Hz()
		, _btlCycles()
		, _canIdBroadcast()
		, _canIdMaster()
		, _canIdSlave()
		, _daqListCanIds()
		, _isMaxDlcRequired(false)
		, _samplePoint_percentage()
		, _sampleRate(XcpCanSampleRate::None)
		, _sjw()
		, _syncEdge(XcpCanSyncEdge::None)
		, _version(0)
	{
	}

	XcpXcpOnCan::XcpXcpOnCan()
		: XcpCanParameters()
		, _parameters()
	{
	}

	McdIdent const XcpPlusIfData::Name({ McdIdentPart("XCPplus") });

	const McdIdent & XcpPlusIfData::name() const
	{
		return XcpPlusIfData::Name;
	}

}
