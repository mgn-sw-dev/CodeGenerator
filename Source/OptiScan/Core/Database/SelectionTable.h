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
    };
}
