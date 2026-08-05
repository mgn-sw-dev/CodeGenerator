#pragma once

#include <OptiScan/Parser/Ldf/LdfDatabase.h>
#include <filesystem>
#include <string>

namespace OptiScan::Core::Database
{

    class LinDatabase 
    {
    public:
        Parser::Ldf::LdfDatabase _ldfDatabase;
        std::string _name;

        LinDatabase();
        /** */
        void clear();
        /** @throw runtime_error if dbcPath does not exist.
         *  @throw runtime_error if dbcPath could not be opened. */
        void loadLdfDatabase(const std::filesystem::path & ldfPath);
    };
    
}
