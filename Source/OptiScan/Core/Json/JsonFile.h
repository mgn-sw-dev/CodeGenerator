#pragma once
#include <Extern/Nlohmann/json.hpp>
#include <filesystem>

namespace OptiScan::Core::Json
{
    class JsonFile 
    {
    public:
        /** Load Json from file path to nlohmann::json object
         * @throw runtime_error if file does not exist.
         * @throw runtime_error if file could not be opened.
         * @throw runtime_error if JSON parse error. */
        static void loadJsonFile(const std::filesystem::path & jsonPath, nlohmann::json & jsonRootObject);
    };

}
