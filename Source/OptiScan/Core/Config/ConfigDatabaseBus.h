#pragma once

#include <cstdint>
#include <string>

namespace OptiScan::Core::Config
{
	class BusObject
	{
	public:
		uint8_t _hardwareId;
		std::string _name;

		BusObject();
		virtual ~BusObject() = default;
		virtual void clear();
		void setIdSuffix(const std::string & idSuffix);
	private:
		bool checkIfIdEndsWith(const std::string & ending) const;
	};
}
