#pragma once

#include <OptiScan/Core/Database/DbcDatabase.h>
#include <filesystem>
#include <string>

namespace OptiScan::Core::Database
{
    class CanDatabase 
    {
    public:
        DbcDatabase _dbcDatabase;
        std::string _name;

        CanDatabase();
        /** */
        void clear();
        /** @throw runtime_error if dbcPath does not exist.
         *  @throw runtime_error if dbcPath could not be opened. */
        void loadDbcDatabase(const std::filesystem::path & dbcPath);
    };

}
