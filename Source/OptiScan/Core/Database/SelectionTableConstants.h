#pragma once

#include <string>

namespace OptiScan::Core::Database
{
	class SelectionTableConstants
	{
	public:
		static inline const std::string CanDbc = "canDbc";
		static inline const std::string CanMessage = "canMessage";
		static inline const std::string DisplayName = "displayName";
		static inline const std::string FrameId = "frameId";
		static inline const std::string FileName = "fileName";
		static inline const std::string Id = "id";
		static inline const std::string LinFrame = "linFrame";
		static inline const std::string LinLdf = "linLdf";
		static inline const std::string Name = "name";
		static inline const std::string SampleRateInHz = "sampleRateInHz";
		static inline const std::string SignalName = "signalName";
		static inline const std::string XcpPlus = "xcpPlus";
		static inline const std::string XcpVariable = "xcpVariable";
	};
}
