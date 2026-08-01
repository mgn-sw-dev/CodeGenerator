#pragma once

#include <string>

namespace OptiScan::Parser::Ldf
{
	class LdfKeyword
	{
	public:
		static inline const std::string AsciiValue = "ascii_value";
		static inline const std::string AssignFrameId = "AssignFrameId";
		static inline const std::string AssignFrameIdRange = "AssignFrameIdRange";
		static inline const std::string AssignNad = "AssignNAD";
		static inline const std::string BcdValue = "bcd_value";
		static inline const std::string ChannelName = "Channel_name";
		static inline const std::string Composite = "composite";
		static inline const std::string ConditionalChangeNad = "ConditionalChangeNAD";
		static inline const std::string ConfigurableFrames = "configurable_frames";
		static inline const std::string Configuration = "configuration";
		static inline const std::string ConfiguredNad = "configured_NAD";
		static inline const std::string DataDump = "DataDump";
		static inline const std::string Delay = "delay";
		static inline const std::string DiagnosticFrames = "Diagnostic_frames";
		static inline const std::string DiagnosticSignals = "Diagnostic_signals";
		static inline const std::string EventTriggeredFrames = "Event_triggered_frames";
		static inline const std::string FaultStateSignals = "fault_state_signals";
		static inline const std::string Frames = "Frames";
		static inline const std::string FreeFormat = "FreeFormat";
		static inline const std::string InitialNad = "initial_NAD";
		static inline const std::string Kbps = "kbps";
		static inline const std::string LinDescriptionFile = "LIN_description_file";
		static inline const std::string LinLanguageVersion = "LIN_language_version";
		static inline const std::string LinProtocol = "LIN_protocol";
		static inline const std::string LinProtocolVersion = "LIN_protocol_version";
		static inline const std::string LinSpeed = "LIN_speed";
		static inline const std::string LogicalValue = "logical_value";
		static inline const std::string Master = "Master";
		static inline const std::string MasterReq = "MasterReq";
		static inline const std::string Ms = "ms";
		static inline const std::string NAsTimeout = "N_As_timeout";
		static inline const std::string NCrTimeout = "N_Cr_timeout";
		static inline const std::string NodeAttributes = "Node_attributes";
		static inline const std::string Nodes = "Nodes";
		static inline const std::string P2Min = "P2_min";
		static inline const std::string PhysicalValue = "physical_value";
		static inline const std::string ProductId = "product_id";
		static inline const std::string ResponseError = "response_error";
		static inline const std::string SaveConfiguration = "SaveConfiguration";
		static inline const std::string ScheduleTables = "Schedule_tables";
		static inline const std::string SignalEncodingTypes = "Signal_encoding_types";
		static inline const std::string SignalRepresentation = "Signal_representation";
		static inline const std::string Signals = "Signals";
		static inline const std::string SlaveResp = "SlaveResp";
		static inline const std::string Slaves = "Slaves";
		static inline const std::string SporadicFrames = "Sporadic_frames";
		static inline const std::string StMin = "ST_min";
	};
}