#pragma once

#include <string>

namespace OptiScan::Parser::Dbc
{
	class DbcKeyword
	{
	public:
		static inline const std::string BA_ = "BA_";
		static inline const std::string BA_DEF_ = "BA_DEF_";
		static inline const std::string BA_DEF_DEF_ = "BA_DEF_DEF_";
		static inline const std::string BO_ = "BO_";
		static inline const std::string BO_TX_BU_ = "BO_TX_BU_";
		static inline const std::string BS_ = "BS_";
		static inline const std::string BU_ = "BU_";
		static inline const std::string CM_ = "CM_";
		static inline const std::string EV_ = "EV_";
		static inline const std::string NS_ = "NS_";
		static inline const std::string SG_ = "SG_";
		static inline const std::string SG_MUL_VAL_ = "SG_MUL_VAL_";
		static inline const std::string SGTYPE_ = "SGTYPE_";
		static inline const std::string VAL_ = "VAL_";
		static inline const std::string VAL_TABLE_ = "VAL_TABLE_";
		static inline const std::string VERSION = "VERSION";
	};
}
