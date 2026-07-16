#pragma once

#include <OptiScan/Core/Database/SelectedSignal.h>
#include <vector>

namespace OptiScan::Core::Database
{

    class SelectionTable 
    {
    public:
        SelectionTable();
    private:
        std::vector<SelectedSignal> _selectionTable;
    
    };

}
