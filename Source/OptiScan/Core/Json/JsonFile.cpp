
#include <OptiScan/Core/Json/JsonFile.h>
#include <fstream>

using namespace nlohmann;
using namespace std;

namespace OptiScan::Core::Json
{
	void JsonFile::loadJsonFile(const std::filesystem::path & jsonPath, json & jsonRootObject)
	{
		if (!filesystem::exists(jsonPath))
		{
			throw runtime_error("JsonFile::loadJsonFile: Json file '" + jsonPath.string() + "' does not exist.");
		}
		ifstream file(jsonPath);
		if (!file.is_open())
		{
			throw runtime_error("JsonFile::loadJsonFile: Could not open json file '" + jsonPath.string() + "'.");
		}
		try
		{
			file >> jsonRootObject;
		}
		catch (const json::parse_error & error)
		{
			throw runtime_error("JsonFile::loadJsonFile: JSON Parse Error: " + string(error.what()));
		}
	}
}
