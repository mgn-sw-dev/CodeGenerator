#pragma once

#include <OptiScan/Core/Config/SystemConfig.h>

namespace OptiScan::Core::Config
{
	template<typename T>
	void SystemConfig::parseArrayAs(const nlohmann::json & array, std::vector<T> & target)
	{
		target.clear();
		target.reserve(array.size());
		for (const auto & element : array)
		{
			target.push_back(element.get<T>());
		}
	}

	template<typename T, typename Parser>
	void SystemConfig::parseArrayWith(const nlohmann::json & array, std::vector<T> & target, Parser parser)
	{
		target.clear();
		target.reserve(array.size());
		for (const auto & element : array)
		{
			target.push_back(parser(element));
		}
	}

	template <typename T>
	void SystemConfig::parseOptionalValue(const nlohmann::json & json, const std::string & key, std::optional<T> & target)
	{
		if (json.contains(key))
		{
			target = json[key].get<T>();
		}
	}
}
