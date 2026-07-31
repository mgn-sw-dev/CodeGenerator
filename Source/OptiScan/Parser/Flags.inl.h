#pragma once

#include <OptiScan/Parser/Flags.h>

namespace OptiScan::Parser
{
	template <typename Enum>
	constexpr Flags<typename Enum>::Flags() noexcept
		: _value(0)
	{
	}

	template <typename Enum>
	constexpr Flags<typename Enum>::Flags(Enum flag) noexcept
		: _value(static_cast<UnderlyingType>(flag))
	{
	}

	template <typename Enum>
	constexpr bool Flags<typename Enum>::testFlag(Enum flag) const noexcept
	{
		UnderlyingType const mask = static_cast<UnderlyingType>(flag);
		return mask == 0 ? _value == 0 : (_value & mask) == mask;
	}

	template <typename Enum>
	void Flags<typename Enum>::setFlag(Enum flag, bool on) noexcept
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
