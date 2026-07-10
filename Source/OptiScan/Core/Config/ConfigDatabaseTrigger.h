#pragma once



namespace OptiScan::Core::Config
{
	enum class TriggerType : uint8_t
	{
		Unknown                     = 0,
		VoiceToCanButton            = 1,
		MessageIdPrefixPartialMatch = 2,
		MessagePayloadPartialMatch  = 3,
		MessageDtcRotmeldungMatch   = 4,
	};

	inline TriggerType triggerTypeFromString(const std::string & triggerTypeString)
	{
		TriggerType type;
		if (triggerTypeString == "VoiceToCanButton")
		{
			type = TriggerType::VoiceToCanButton;
		}
		else if (triggerTypeString == "MessageIdPrefixPartialMatch")
		{
			type = TriggerType::MessageIdPrefixPartialMatch;
		}
		else if (triggerTypeString == "MessagePayloadPartialMatch")
		{
			type = TriggerType::MessagePayloadPartialMatch;
		}
		else if (triggerTypeString == "MessageDtcRotmeldungMatch")
		{
			type = TriggerType::MessageDtcRotmeldungMatch;
		}
		else
		{
			throw std::runtime_error("Unknown trigger type: " + triggerTypeString);
		}
		return type;
	}

	struct CanTriggerSignal
	{
		std::string _bus;
		std::string _dbcName;
		std::string _messageName;
		std::string _signalName;
	};

	class Trigger
	{
	public:
		std::string _bus;
		uint32_t _messageId = 0;
		std::vector<CanTriggerSignal> _signals;
		uint32_t _triggerValue = 0;
		uint32_t _triggerValueOffset = 0;
		uint32_t _triggerValueSize = 0;
		TriggerType _type = TriggerType::Unknown;

		Trigger() = default;
	};

}
