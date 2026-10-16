#pragma once

#include <string>

namespace OptiScan::Parser::A2l::IfData::Xcp
{
	/** Keywords of the XCP IF_DATA extension. Kept apart from A2lKeyword, which holds
	 *  the ASAP2 keywords of the core parser. */
	class XcpKeyword
	{
	public:
		static inline const std::string Baudrate = "BAUDRATE";
		static inline const std::string Block = "BLOCK";
		static inline const std::string BtlCycles = "BTL_CYCLES";
		static inline const std::string CanIdBroadcast = "CAN_ID_BROADCAST";
		static inline const std::string CanIdMaster = "CAN_ID_MASTER";
		static inline const std::string CanIdSlave = "CAN_ID_SLAVE";
		static inline const std::string CommunicationModeSupported = "COMMUNICATION_MODE_SUPPORTED";
		static inline const std::string Daq = "DAQ";
		static inline const std::string DaqEvent = "DAQ_EVENT";
		static inline const std::string DaqList = "DAQ_LIST";
		static inline const std::string DaqListCanId = "DAQ_LIST_CAN_ID";
		static inline const std::string Dynamic = "DYNAMIC";
		static inline const std::string Event = "EVENT";
		static inline const std::string Fixed = "FIXED";
		static inline const std::string FreezeSupported = "FREEZE_SUPPORTED";
		static inline const std::string Interleaved = "INTERLEAVED";
		static inline const std::string Master = "MASTER";
		static inline const std::string MaxDlcRequired = "MAX_DLC_REQUIRED";
		static inline const std::string OptionalCmd = "OPTIONAL_CMD";
		static inline const std::string Pag = "PAG";
		static inline const std::string Pgm = "PGM";
		static inline const std::string PidOffSupported = "PID_OFF_SUPPORTED";
		static inline const std::string PrescalerSupported = "PRESCALER_SUPPORTED";
		static inline const std::string ProtocolLayer = "PROTOCOL_LAYER";
		static inline const std::string ResumeSupported = "RESUME_SUPPORTED";
		static inline const std::string SamplePoint = "SAMPLE_POINT";
		static inline const std::string SampleRate = "SAMPLE_RATE";
		static inline const std::string SeedAndKeyExternalFunction = "SEED_AND_KEY_EXTERNAL_FUNCTION";
		static inline const std::string Segment = "SEGMENT";
		static inline const std::string Sjw = "SJW";
		static inline const std::string Slave = "SLAVE";
		static inline const std::string Static = "STATIC";
		static inline const std::string Stim = "STIM";
		static inline const std::string SyncEdge = "SYNC_EDGE";
		static inline const std::string TimestampSupported = "TIMESTAMP_SUPPORTED";
		static inline const std::string TransportLayerInstance = "TRANSPORT_LAYER_INSTANCE";
		static inline const std::string Variable = "VARIABLE";
		static inline const std::string XcpOnCan = "XCP_ON_CAN";

	private:
		XcpKeyword() = default;
	};

}
