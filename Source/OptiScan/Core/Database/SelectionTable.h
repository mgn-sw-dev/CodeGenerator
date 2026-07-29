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
        std::vector<SelectedSignal> _selectedSignals;

        SelectionTable();
        virtual ~SelectionTable() = default;
        /** */
        void clear();
        /** */
        bool hasValue() const;
        /** @throw runtime_error if selection table file does not exist. */
        void loadJsonFile(const std::filesystem::path & selectionTablePath);
        /** */
        virtual void parse() = 0;

    protected:
        nlohmann::json _jsonRootObject;
    };

    class CanSelectionTable : public SelectionTable
    {
    public:
        CanSelectionTable();
        /** */
        virtual void parse() override;
    };

    class LinSelectionTable : public SelectionTable
    {
    public:
        LinSelectionTable();
        /** */
        virtual void parse() override;
    };

    class XcpSelectionTable : public SelectionTable
    {
    public:
        XcpSelectionTable();
        /** */
        virtual void parse() override;
    };

}
