#pragma once

#include <string>

namespace OptiScan::Parser::Arxml
{
	/** Element names of the ARXML schema, same role as A2lKeyword for ASAP2. */
	class ArxmlKeyword
	{
	public:
		static inline const std::string AdminData = "ADMIN-DATA";
		static inline const std::string ArPackage = "AR-PACKAGE";
		static inline const std::string ArPackages = "AR-PACKAGES";
		static inline const std::string Autosar = "AUTOSAR";
		static inline const std::string Elements = "ELEMENTS";
		static inline const std::string ShortName = "SHORT-NAME";

	private:
		ArxmlKeyword() = default;
	};

}
