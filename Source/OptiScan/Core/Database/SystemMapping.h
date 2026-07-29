#pragma once

#include <string>
#include <vector>

namespace OptiScan::Core::Database
{

    struct MappingCanObject
    {
        std::string _busName;
        std::vector<std::string> _vmmNames;
    };

    struct MappingLinObject
    {
        std::string _busName;
        std::string _ldfName;
    };

    class SystemMapping 
    {
    public:
        SystemMapping();
        /** */
        void clear();
        /** */
        bool getCanMapping(uint8_t hardwareId, MappingCanObject & mapping) const;
        /** */
        bool getLinMapping(uint8_t hardwareId, MappingLinObject & mapping) const;
        /** */
        void setCanMapping(uint8_t hardwareId, const MappingCanObject & mapping);
        /** */
        void setLinMapping(uint8_t hardwareId, const MappingLinObject & mapping);

    private:
        constexpr static uint8_t MaxCans = 20;
        constexpr static uint8_t MaxLins = 3;
        std::vector<MappingCanObject> _canMappings;
        std::vector<MappingLinObject> _linMappings;

    };

}
