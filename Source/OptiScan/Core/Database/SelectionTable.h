#pragma once

#include <OptiScan/Core/Database/SelectedSignal.h>
#include <Extern/Nlohmann/json.hpp>
#include <filesystem>
#include <vector>

namespace OptiScan::Core::Database
{

    class SelectionTable 
    {
    public:
        std::vector<SelectedSignal> _selectionTable;

        SelectionTable();
        /** @throw runtime_error if selection table file does not exist. */
        void loadJsonFile(const std::filesystem::path & selectionTablePath);
        /** */
        void parse();
        /** */
        void clear();

    private:
        nlohmann::json _jsonRootObject;
        /** Open file and parse as nlohmann::json object.
         * @throw runtime_error: If file failed to open.
         * @throw runtime_error: If JSON Parse Error.
         * @todo: Same method like in System config: Maby common json class with static method. */
        static void loadFromJson(const std::string & filePath, nlohmann::json & jsonRootObject);
    
    };
}
