#pragma once

#include <OptiScan/Parser/Flags.h>

namespace OptiScan::Parser
{
	template <typename Enum>
	constexpr Flags<Enum>::Flags() noexcept
		: _value(0)
	{
	}

	template <typename Enum>
	constexpr Flags<Enum>::Flags(Enum flag) noexcept
		: _value(static_cast<UnderlyingType>(flag))
	{
	}

	template <typename Enum>
	constexpr bool Flags<Enum>::testFlag(Enum flag) const noexcept
	{
		UnderlyingType const mask = static_cast<UnderlyingType>(flag);
		bool result = (_value & mask) == mask;
		if (mask == 0)
		{
			result = _value == 0;
		}
		return result;
	}

	template <typename Enum>
	void Flags<Enum>::setFlag(Enum flag, bool on) noexcept
	{
		UnderlyingType const mask = static_cast<UnderlyingType>(flag);
		if (on)
		{
			_value |= mask;
		}
		else
		{
			_value &= static_cast<UnderlyingType>(~mask);
		}
	}

}
