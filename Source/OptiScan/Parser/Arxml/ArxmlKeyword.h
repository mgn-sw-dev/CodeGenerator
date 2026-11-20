#pragma once

#include <string>

namespace OptiScan::Parser::Arxml
{
	/** Element and attribute names of the ARXML schema, same role as A2lKeyword for
	 *  ASAP2. */
	class ArxmlKeyword
	{
	public:
		static inline const std::string AdminData = "ADMIN-DATA";
		static inline const std::string ArPackage = "AR-PACKAGE";
		static inline const std::string ArPackages = "AR-PACKAGES";
		static inline const std::string Autosar = "AUTOSAR";
		static inline const std::string BaseTypeEncoding = "BASE-TYPE-ENCODING";
		static inline const std::string BaseTypeRef = "BASE-TYPE-REF";
		static inline const std::string BaseTypeSize = "BASE-TYPE-SIZE";
		static inline const std::string Category = "CATEGORY";
		static inline const std::string CompuConst = "COMPU-CONST";
		static inline const std::string CompuDenominator = "COMPU-DENOMINATOR";
		static inline const std::string CompuInternalToPhys = "COMPU-INTERNAL-TO-PHYS";
		static inline const std::string CompuMethod = "COMPU-METHOD";
		static inline const std::string CompuMethodRef = "COMPU-METHOD-REF";
		static inline const std::string CompuNumerator = "COMPU-NUMERATOR";
		static inline const std::string CompuRationalCoeffs = "COMPU-RATIONAL-COEFFS";
		static inline const std::string CompuScale = "COMPU-SCALE";
		static inline const std::string CompuScales = "COMPU-SCALES";
		static inline const std::string DataConstr = "DATA-CONSTR";
		static inline const std::string DataConstrRef = "DATA-CONSTR-REF";
		static inline const std::string DataConstrRule = "DATA-CONSTR-RULE";
		static inline const std::string DataConstrRules = "DATA-CONSTR-RULES";
		static inline const std::string Desc = "DESC";
		static inline const std::string Dest = "DEST";
		static inline const std::string DisplayName = "DISPLAY-NAME";
		static inline const std::string DynamicLength = "DYNAMIC-LENGTH";
		static inline const std::string Elements = "ELEMENTS";
		static inline const std::string InitValue = "INIT-VALUE";
		static inline const std::string InternalConstrs = "INTERNAL-CONSTRS";
		static inline const std::string InvalidValue = "INVALID-VALUE";
		static inline const std::string ISignal = "I-SIGNAL";
		static inline const std::string L = "L";
		static inline const std::string L2 = "L-2";
		static inline const std::string Length = "LENGTH";
		static inline const std::string LowerLimit = "LOWER-LIMIT";
		static inline const std::string NetworkRepresentationProps = "NETWORK-REPRESENTATION-PROPS";
		static inline const std::string NumericalValueSpecification = "NUMERICAL-VALUE-SPECIFICATION";
		static inline const std::string PhysConstrs = "PHYS-CONSTRS";
		static inline const std::string PhysicalProps = "PHYSICAL-PROPS";
		static inline const std::string ShortLabel = "SHORT-LABEL";
		static inline const std::string ShortName = "SHORT-NAME";
		static inline const std::string SwBaseType = "SW-BASE-TYPE";
		static inline const std::string SwDataDefPropsConditional = "SW-DATA-DEF-PROPS-CONDITIONAL";
		static inline const std::string SwDataDefPropsVariants = "SW-DATA-DEF-PROPS-VARIANTS";
		static inline const std::string Symbol = "SYMBOL";
		static inline const std::string SystemSignal = "SYSTEM-SIGNAL";
		static inline const std::string SystemSignalRef = "SYSTEM-SIGNAL-REF";
		static inline const std::string Unit = "UNIT";
		static inline const std::string UnitRef = "UNIT-REF";
		static inline const std::string UpperLimit = "UPPER-LIMIT";
		static inline const std::string V = "V";
		static inline const std::string Value = "VALUE";
		static inline const std::string Vt = "VT";

	private:
		ArxmlKeyword() = default;
	};

}
