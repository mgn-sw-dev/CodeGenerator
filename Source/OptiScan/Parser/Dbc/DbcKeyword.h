#pragma once

#include <string>
#include <unordered_map>

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
		static inline const std::string SIG_GROUP_ = "SIG_GROUP_";
		static inline const std::string VAL_ = "VAL_";
		static inline const std::string VAL_TABLE_ = "VAL_TABLE_";
		static inline const std::string VERSION = "VERSION";
	};

	enum class DbcKeywordKind
	{
		Unknown,
		BA_,
		BA_DEF_,
		BA_DEF_DEF_,
		BO_,
		BO_TX_BU_,
		BS_,
		BU_,
		CM_,
		EV_,
		NS_,
		SG_,
		SG_MUL_VAL_,
		SGTYPE_,
		SIG_GROUP_,
		VAL_,
		VAL_TABLE_,
		VERSION,
	};

	static const std::unordered_map<std::string, DbcKeywordKind> KeywordMap =
	{
		{ DbcKeyword::BA_, DbcKeywordKind::BA_ },
		{DbcKeyword::BA_DEF_, DbcKeywordKind::BA_DEF_ },
		{DbcKeyword::BA_DEF_DEF_, DbcKeywordKind::BA_DEF_DEF_ },
		{DbcKeyword::BO_, DbcKeywordKind::BO_ },
		{DbcKeyword::BO_TX_BU_, DbcKeywordKind::BO_TX_BU_ },
		{DbcKeyword::BS_, DbcKeywordKind::BS_ },
		{DbcKeyword::BU_, DbcKeywordKind::BU_ },
		{DbcKeyword::CM_, DbcKeywordKind::CM_ },
		{DbcKeyword::EV_, DbcKeywordKind::EV_ },
		{DbcKeyword::NS_, DbcKeywordKind::NS_ },
		{DbcKeyword::SG_, DbcKeywordKind::SG_ },
		{DbcKeyword::SG_MUL_VAL_, DbcKeywordKind::SG_MUL_VAL_ },
		{DbcKeyword::SGTYPE_, DbcKeywordKind::SGTYPE_ },
		{DbcKeyword::SIG_GROUP_, DbcKeywordKind::SIG_GROUP_ },
		{DbcKeyword::VAL_, DbcKeywordKind::VAL_ },
		{DbcKeyword::VAL_TABLE_, DbcKeywordKind::VAL_TABLE_ },
		{DbcKeyword::VERSION, DbcKeywordKind::VERSION },
	};
}
