#pragma once

#include <string>

namespace OptiScan::Parser::A2l
{
	class A2lKeyword
	{
	public:
		static inline const std::string A2ml = "A2ML";
		static inline const std::string A2mlVersion = "A2ML_VERSION";
		static inline const std::string Asap2Version = "ASAP2_VERSION";
		static inline const std::string CompuMethod = "COMPU_METHOD";
		static inline const std::string CompuTab = "COMPU_TAB";
		static inline const std::string CompuVTab = "COMPU_VTAB";
		static inline const std::string CompuVTabRange = "COMPU_VTAB_RANGE";
		static inline const std::string DefaultValue = "DEFAULT_VALUE";
		static inline const std::string DefaultValueNumeric = "DEFAULT_VALUE_NUMERIC";
		static inline const std::string Formula = "FORMULA";
		static inline const std::string Header = "HEADER";
		static inline const std::string IfData = "IF_DATA";
		static inline const std::string Measurement = "MEASUREMENT";
		static inline const std::string ModBus = "MODBUS";
		static inline const std::string ModCommon = "MOD_COMMON";
		static inline const std::string ModPar = "MOD_PAR";
		static inline const std::string Module = "MODULE";
		static inline const std::string Project = "PROJECT";
		static inline const std::string ProjectNo = "PROJECT_NO";
		static inline const std::string VariantCoding = "VARIANT_CODING";
		static inline const std::string Version = "VERSION";

	private:
		A2lKeyword();
	};	
}
