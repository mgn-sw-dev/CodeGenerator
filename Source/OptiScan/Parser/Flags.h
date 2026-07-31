#pragma once
#include <type_traits>

namespace OptiScan::Parser
{
	/** Schlankes Bitmask-Werkzeug für "enum class"-Flags.
	 *  Erwartet Enum-Werte als Bitmaske (1 << 0, 1 << 1, ...). */
	template <typename Enum>
	class Flags
	{
	public:
		using UnderlyingType = std::underlying_type_t<Enum>;

		constexpr Flags() noexcept;
		constexpr Flags(Enum flag) noexcept;
		/** */
		constexpr bool testFlag(Enum flag) const noexcept;
		/** */
		void setFlag(Enum flag, bool on = true) noexcept;

	private:
		UnderlyingType _value;
	};
}
#include <OptiScan/Parser/Flags.inl.h>
