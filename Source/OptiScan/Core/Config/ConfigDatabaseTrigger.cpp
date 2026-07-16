
#include <OptiScan/Core/Config/ConfigDatabaseTrigger.h>

namespace OptiScan::Core::Config
{
	void Trigger::clear()
	{
		this->_bus.clear();
		this->_messageId = 0;
		this->_signals.clear();
		this->_triggerValue = 0;
		this->_triggerValueOffset = 0;
		this->_triggerValueSize = 0;
		this->_type = TriggerType::Unknown;
	}

	bool Trigger::isValid() const
	{
		bool result = false;
		switch (this->_type)
		{
		case TriggerType::VoiceToCanButton:
			result = true;
			break;
		case TriggerType::MessageIdPrefixPartialMatch:
			result = this->_bus.size() > 0 && this->_messageId > 0;
			break;
		case TriggerType::MessagePayloadPartialMatch:
		case TriggerType::MessageDtcRotmeldungMatch:
			result = this->_bus.size() > 0 &&
					this->_messageId > 0 &&
					this->_triggerValue > 0 &&
					this->_triggerValueOffset > 0 &&
					this->_triggerValueSize > 0;
			break;
		case TriggerType::Unknown:
		default:
			break;
		}
		return result;
	}

	void Trigger::setTriggerType(const TriggerType type)
	{
		this->_type = type;
	}

	void Trigger::setTriggerType(const std::string & typeString)
	{
		TriggerType type;
		if (typeString == "VoiceToCanButton")
		{
			type = TriggerType::VoiceToCanButton;
		}
		else if (typeString == "MessageIdPrefixPartialMatch")
		{
			type = TriggerType::MessageIdPrefixPartialMatch;
		}
		else if (typeString == "MessagePayloadPartialMatch")
		{
			type = TriggerType::MessagePayloadPartialMatch;
		}
		else if (typeString == "MessagePegasusDtcRotmeldungMatch")
		{
			type = TriggerType::MessageDtcRotmeldungMatch;
		}
		else
		{
			type = TriggerType::Unknown;
		}
		this->setTriggerType(type);
	}
}
