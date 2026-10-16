#pragma once

#include <OptiScan/Parser/A2l/IfData/Xcp/XcpEnums.h>
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpTimeUnit.h>
#include <OptiScan/Parser/A2l/Mcd/McdIdent.h>
#include <OptiScan/Parser/A2l/Mcd/McdIfData.h>
#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <variant>
#include <vector>

namespace OptiScan::Parser::A2l::IfData::Xcp
{
    /** CAN identifier as written in the A2L. Bit 31 marks an extended identifier. */
    class XcpCanId
    {
    public:
        static const uint32_t ExtendedMask = 0x7FFF'FFFF;
        uint32_t _value = 0;
        /** Identifier without the extended marker bit. */
        uint32_t extendedId() const;
        /** */
        bool isExtended() const;
    };

    struct XcpCommunicationModeBlock
    {
        bool _masterIsSupported;
        uint8_t _masterMaxBs;
        uint8_t _masterMaxSt;
        bool _slaveIsSupported;
        /** */
        XcpCommunicationModeBlock();
    };

    struct XcpCommunicationModeInterleaved
    {
        uint8_t _queueSize;
        /** */
        XcpCommunicationModeInterleaved();
    };

    /** Replaces the clone based class hierarchy of the Qt version. */
    using XcpCommunicationMode = std::variant<XcpCommunicationModeBlock, XcpCommunicationModeInterleaved>;

    struct XcpEvent
    {
        bool _canDaq;
        bool _canStim;
        std::string _eventChannelName;
        uint16_t _eventChannelNumber;
        std::string _eventChannelShortName;
        uint8_t _maxDaqList;
        uint8_t _priority;
        uint8_t _timeCycle;
        XcpTimeUnit _timeUnit;
        /** */
        XcpEvent();
    };

    struct XcpDaq
    {
        XcpAddressExtension _addressExtension;
        std::vector<XcpEvent> _events;
        XcpGranularityOdtEntrySizeDaq _granularityOdtEntrySizeDaq;
        XcpIdentificationFieldType _identificationField;
        bool _isDynamic;
        bool _isPidOffSupported;
        bool _isPrescalerSupported;
        bool _isResumeSupported;
        uint16_t _maxDaq;
        uint16_t _maxEventChannel;
        uint8_t _maxOdtEntrySizeDaq;
        uint8_t _minDaq;
        XcpOptimizationType _optimizationType;
        XcpOverloadIndication _overloadIndication;
        /** */
        XcpDaq();
    };

    struct XcpDaqListCanId
    {
        uint16_t _daqListNumber;
        std::optional<XcpCanId> _fixedTo;
        bool _isVariable;
        /** */
        XcpDaqListCanId();
    };

    /** PAG block: calibration page handling.
     *  Parsed in addition to the old parser, which skipped the content. */
    struct XcpPag
    {
        bool _isFreezeSupported;
        uint8_t _maxSegments;
        /** */
        XcpPag();
    };

    struct XcpProtocolLayer
    {
        XcpAddressGranularity _addressGranularity;
        XcpByteOrder _byteOrder;
        std::optional<XcpCommunicationMode> _communicationModeSupported;
        uint8_t _maxCto;
        uint16_t _maxDto;
        std::set<XcpOptionalCommand> _optionalCommands;
        std::optional<std::string> _seedAndKeyExternalFunction;
        uint16_t _t1_ms;
        uint16_t _t2_ms;
        uint16_t _t3_ms;
        uint16_t _t4_ms;
        uint16_t _t5_ms;
        uint16_t _t6_ms;
        uint16_t _t7_ms;
        uint16_t _version;
        /** */
        XcpProtocolLayer();
    };

    /** Parameters that may appear both at IF_DATA level, as a default, and inside a
     *  transport layer block, as an override. */
    struct XcpCommonParameters
    {
        std::optional<XcpDaq> _daq;
        std::optional<XcpPag> _pag;
        std::optional<XcpProtocolLayer> _protocolLayer;
        /** */
        XcpCommonParameters();
        /** Values present in @p source replace the ones of this instance. */
        XcpCommonParameters overruled(const XcpCommonParameters & source) const;
    };

    struct XcpCanParameters
    {
        std::optional<uint32_t> _baudrate_Hz;
        std::optional<uint8_t> _btlCycles;
        std::optional<XcpCanId> _canIdBroadcast;
        std::optional<XcpCanId> _canIdMaster;
        std::optional<XcpCanId> _canIdSlave;
        std::vector<XcpDaqListCanId> _daqListCanIds;
        bool _isMaxDlcRequired;
        std::optional<uint8_t> _samplePoint_percentage;
        XcpCanSampleRate _sampleRate;
        std::optional<uint8_t> _sjw;
        XcpCanSyncEdge _syncEdge;
        uint16_t _version;
        /** */
        XcpCanParameters();
    };

    struct XcpXcpOnCan : public XcpCanParameters
    {
        /** Overrides the defaults given at IF_DATA level. */
        XcpCommonParameters _parameters;
        /** */
        XcpXcpOnCan();
    };

    /** Shared base of the classic XCP and the XCPplus IF_DATA content. */
    class XcpIfDataBase : public Mcd::McdIfData
    {
    public:
        XcpCommonParameters _parameters;
    };

    struct XcpPlusXcpOnCan : public XcpXcpOnCan
    {
        std::optional<std::string> _transportLayerInstance;
    };

    /** Content of an "IF_DATA XCPplus" block. */
    class XcpPlusIfData : public XcpIfDataBase
    {
    public:
        static const Mcd::McdIdent Name;
        uint16_t _marker = 0;
        std::vector<XcpPlusXcpOnCan> _xcpOnCans;
        /** */
        const Mcd::McdIdent & name() const override;
    };

}
