#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace OptiScan::Parser::A2l::Mcd
{
    /** SYSTEM_CONSTANT of a MOD_PAR block. */
    struct McdSystemConstant
    {
        std::string _name;
        std::string _value;
    };

    /** MOD_PAR block. Mostly descriptive metadata about the ECU and the project. */
    struct McdModuleParameters
    {
        std::vector<uint32_t> _addrEpks;
        std::string _comment;
        std::string _cpuType;
        std::string _customer;
        std::string _customerNumber;
        std::string _ecu;
        int32_t _ecuCalibrationOffset;
        std::string _epk;
        uint16_t _numberOfInterfaces;
        std::string _phoneNumber;
        std::string _supplier;
        std::vector<McdSystemConstant> _systemConstants;
        std::string _user;
        std::string _version;
        /** */
        McdModuleParameters();
    };

}
