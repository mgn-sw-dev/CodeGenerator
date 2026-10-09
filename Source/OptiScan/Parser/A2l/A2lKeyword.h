#pragma once

#include <string>

namespace OptiScan::Parser::A2l
{
	class A2lKeyword
	{
	public:
		static inline const std::string A2ml = "A2ML";
		static inline const std::string A2mlVersion = "A2ML_VERSION";
		static inline const std::string AddrEpk = "ADDR_EPK";
		static inline const std::string AlignmentByte = "ALIGNMENT_BYTE";
		static inline const std::string AlignmentFloat16Ieee = "ALIGNMENT_FLOAT16_IEEE";
		static inline const std::string AlignmentFloat32Ieee = "ALIGNMENT_FLOAT32_IEEE";
		static inline const std::string AlignmentFloat64Ieee = "ALIGNMENT_FLOAT64_IEEE";
		static inline const std::string AlignmentInt64 = "ALIGNMENT_INT64";
		static inline const std::string AlignmentLong = "ALIGNMENT_LONG";
		static inline const std::string AlignmentWord = "ALIGNMENT_WORD";
		static inline const std::string Asap2Version = "ASAP2_VERSION";
		static inline const std::string ByteOrder = "BYTE_ORDER";
		static inline const std::string Coeffs = "COEFFS";
		static inline const std::string CoeffsLinear = "COEFFS_LINEAR";
		static inline const std::string CompuMethod = "COMPU_METHOD";
		static inline const std::string CompuTab = "COMPU_TAB";
		static inline const std::string CompuTabRef = "COMPU_TAB_REF";
		static inline const std::string CompuVTab = "COMPU_VTAB";
		static inline const std::string CompuVTabRange = "COMPU_VTAB_RANGE";
		static inline const std::string CpuType = "CPU_TYPE";
		static inline const std::string Customer = "CUSTOMER";
		static inline const std::string CustomerNo = "CUSTOMER_NO";
		static inline const std::string DataSize = "DATA_SIZE";
		static inline const std::string DefaultValue = "DEFAULT_VALUE";
		static inline const std::string DefaultValueNumeric = "DEFAULT_VALUE_NUMERIC";
		static inline const std::string Deposit = "DEPOSIT";
		static inline const std::string Ecu = "ECU";
		static inline const std::string EcuCalibrationOffset = "ECU_CALIBRATION_OFFSET";
		static inline const std::string Epk = "EPK";
		static inline const std::string Formula = "FORMULA";
		static inline const std::string Header = "HEADER";
		static inline const std::string IfData = "IF_DATA";
		static inline const std::string Measurement = "MEASUREMENT";
		static inline const std::string ModCommon = "MOD_COMMON";
		static inline const std::string ModPar = "MOD_PAR";
		static inline const std::string Module = "MODULE";
		static inline const std::string NoOfInterfaces = "NO_OF_INTERFACES";
		static inline const std::string PhoneNo = "PHONE_NO";
		static inline const std::string Project = "PROJECT";
		static inline const std::string ProjectNo = "PROJECT_NO";
		static inline const std::string RefUnit = "REF_UNIT";
		static inline const std::string StatusStringRef = "STATUS_STRING_REF";
		static inline const std::string Supplier = "SUPPLIER";
		static inline const std::string SystemConstant = "SYSTEM_CONSTANT";
		static inline const std::string User = "USER";
		static inline const std::string VariantCoding = "VARIANT_CODING";
		static inline const std::string Version = "VERSION";

	private:
		A2lKeyword();
	};	
}
