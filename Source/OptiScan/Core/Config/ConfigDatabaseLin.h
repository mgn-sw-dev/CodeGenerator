#pragma once

#include <OptiScan/Core/Config/ConfigDatabaseBus.h>

namespace OptiScan::Core::Config
{

    class LinBusObject : public BusObject
    {
    public:
        std::string _ldfName;

        LinBusObject() = default;
        void clear() override;
    };

}
