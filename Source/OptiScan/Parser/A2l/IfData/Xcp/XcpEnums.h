#pragma once

#include <cstdint>
#include <map>
#include <string>

namespace OptiScan::Parser::A2l::IfData::Xcp
{
    enum class XcpAddressExtension
    {
        Free = 0,
        Odt = 1,
        Daq = 3,
    };

    enum class XcpAddressGranularity
    {
        Byte = 1,
        Word = 2,
        DWord = 4,
    };

    enum class XcpByteOrder
    {
        MsbLast = 0,
        MsbFirst = 1,
    };

    enum class XcpCanSampleRate
    {
        None = 0,
        Single = 1,
        Triple = 3,
    };

    enum class XcpCanSyncEdge
    {
        None = 0,
        Single = 1,
        Dual = 2,
    };

    /** Kind of an event channel as written in a DAQ EVENT block. */
    enum class XcpEventKind
    {
        Daq = 1,
        Stim = 2,
        DaqStim = 3,
    };

    enum class XcpGranularityOdtEntrySizeDaq
    {
        Byte = 1,
        Word = 2,
        DWord = 4,
        DLong = 8,
    };

    enum class XcpIdentificationFieldType
    {
        Absolute = 0,
        RelativeByte = 1,
        RelativeWord = 2,
        RelativeWordAligned = 3,
    };

    enum class XcpOptimizationType
    {
        Default = 0,
        OdtType16 = 1,
        OdtType32 = 2,
        OdtType64 = 3,
        OdtTypeAlignment = 4,
        MaxEntrySize = 5,
    };

    enum class XcpOptionalCommand : uint8_t
    {
        GetCommModeInfo = 0xFB,
        GetId = 0xFA,
        SetRequest = 0xF9,
        GetSeed = 0xF8,
        Unlock = 0xF7,
        SetMta = 0xF6,
        Upload = 0xF5,
        ShortUpload = 0xF4,
        BuildChecksum = 0xF3,
        TransportLayerCmd = 0xF2,
        UserCmd = 0xF1,
        Download = 0xF0,
        DownloadNext = 0xEF,
        DownloadMax = 0xEE,
        ShortDownload = 0xED,
        ModifyBits = 0xEC,
        SetCalPage = 0xEB,
        GetCalPage = 0xEA,
        GetPagProcessorInfo = 0xE9,
        GetSegmentInfo = 0xE8,
        GetPageInfo = 0xE7,
        SetSegmentMode = 0xE6,
        GetSegmentMode = 0xE5,
        CopyCalPage = 0xE4,
        ClearDaqList = 0xE3,
        SetDaqPtr = 0xE2,
        WriteDaq = 0xE1,
        SetDaqListMode = 0xE0,
        GetDaqListMode = 0xDF,
        StartStopDaqList = 0xDE,
        StartStopSynch = 0xDD,
        GetDaqClock = 0xDC,
        ReadDaq = 0xDB,
        GetDaqProcessorInfo = 0xDA,
        GetDaqResolutionInfo = 0xD9,
        GetDaqListInfo = 0xD8,
        GetDaqEventInfo = 0xD7,
        FreeDaq = 0xD6,
        AllocDaq = 0xD5,
        AllocOdt = 0xD4,
        AllocOdtEntry = 0xD3,
        ProgramStart = 0xD2,
        ProgramClear = 0xD1,
        Program = 0xD0,
        ProgramReset = 0xCF,
        GetPgmProcessorInfo = 0xCE,
        GetSectorInfo = 0xCD,
        ProgramPrepare = 0xCC,
        ProgramFormat = 0xCB,
        ProgramNext = 0xCA,
        ProgramMax = 0xC9,
        ProgramVerify = 0xC8,
    };

    enum class XcpOverloadIndication
    {
        None = 0,
        Pid = 1,
        Event = 2,
    };

    /** Text to value maps used with A2lTokenReader::parseEnumValue. */
    class XcpEnumText
    {
    public:
        static inline const std::map<std::string, XcpAddressExtension> AddressExtension =
        {
            { "ADDRESS_EXTENSION_DAQ",  XcpAddressExtension::Daq },
            { "ADDRESS_EXTENSION_FREE", XcpAddressExtension::Free },
            { "ADDRESS_EXTENSION_ODT",  XcpAddressExtension::Odt },
        };

        static inline const std::map<std::string, XcpAddressGranularity> AddressGranularity =
        {
            { "ADDRESS_GRANULARITY_BYTE",  XcpAddressGranularity::Byte },
            { "ADDRESS_GRANULARITY_DWORD", XcpAddressGranularity::DWord },
            { "ADDRESS_GRANULARITY_WORD",  XcpAddressGranularity::Word },
        };

        static inline const std::map<std::string, XcpByteOrder> ByteOrder =
        {
            { "BYTE_ORDER_MSB_FIRST", XcpByteOrder::MsbFirst },
            { "BYTE_ORDER_MSB_LAST",  XcpByteOrder::MsbLast },
        };

        static inline const std::map<std::string, XcpCanSampleRate> CanSampleRate =
        {
            { "SINGLE", XcpCanSampleRate::Single },
            { "TRIPLE", XcpCanSampleRate::Triple },
        };

        static inline const std::map<std::string, XcpCanSyncEdge> CanSyncEdge =
        {
            { "DUAL",   XcpCanSyncEdge::Dual },
            { "SINGLE", XcpCanSyncEdge::Single },
        };

        static inline const std::map<std::string, XcpEventKind> EventKind =
        {
            { "DAQ",      XcpEventKind::Daq },
            { "DAQ_STIM", XcpEventKind::DaqStim },
            { "STIM",     XcpEventKind::Stim },
        };

        static inline const std::map<std::string, XcpGranularityOdtEntrySizeDaq> GranularityOdtEntrySizeDaq =
        {
            { "GRANULARITY_ODT_ENTRY_SIZE_DAQ_BYTE",  XcpGranularityOdtEntrySizeDaq::Byte },
            { "GRANULARITY_ODT_ENTRY_SIZE_DAQ_DLONG", XcpGranularityOdtEntrySizeDaq::DLong },
            { "GRANULARITY_ODT_ENTRY_SIZE_DAQ_DWORD", XcpGranularityOdtEntrySizeDaq::DWord },
            { "GRANULARITY_ODT_ENTRY_SIZE_DAQ_WORD",  XcpGranularityOdtEntrySizeDaq::Word },
        };

        static inline const std::map<std::string, XcpIdentificationFieldType> IdentificationFieldType =
        {
            { "IDENTIFICATION_FIELD_TYPE_ABSOLUTE",              XcpIdentificationFieldType::Absolute },
            { "IDENTIFICATION_FIELD_TYPE_RELATIVE_BYTE",         XcpIdentificationFieldType::RelativeByte },
            { "IDENTIFICATION_FIELD_TYPE_RELATIVE_WORD",         XcpIdentificationFieldType::RelativeWord },
            { "IDENTIFICATION_FIELD_TYPE_RELATIVE_WORD_ALIGNED", XcpIdentificationFieldType::RelativeWordAligned },
        };

        static inline const std::map<std::string, XcpOptimizationType> OptimizationType =
        {
            { "OPTIMISATION_TYPE_DEFAULT",            XcpOptimizationType::Default },
            { "OPTIMISATION_TYPE_MAX_ENTRY_SIZE",     XcpOptimizationType::MaxEntrySize },
            { "OPTIMISATION_TYPE_ODT_TYPE_16",        XcpOptimizationType::OdtType16 },
            { "OPTIMISATION_TYPE_ODT_TYPE_32",        XcpOptimizationType::OdtType32 },
            { "OPTIMISATION_TYPE_ODT_TYPE_64",        XcpOptimizationType::OdtType64 },
            { "OPTIMISATION_TYPE_ODT_TYPE_ALIGNMENT", XcpOptimizationType::OdtTypeAlignment },
        };

        static inline const std::map<std::string, XcpOptionalCommand> OptionalCommand =
        {
            { "ALLOC_DAQ",                 XcpOptionalCommand::AllocDaq },
            { "ALLOC_ODT",                 XcpOptionalCommand::AllocOdt },
            { "ALLOC_ODT_ENTRY",           XcpOptionalCommand::AllocOdtEntry },
            { "BUILD_CHECKSUM",            XcpOptionalCommand::BuildChecksum },
            { "CLEAR_DAQ_LIST",            XcpOptionalCommand::ClearDaqList },
            { "COPY_CAL_PAGE",             XcpOptionalCommand::CopyCalPage },
            { "DOWNLOAD",                  XcpOptionalCommand::Download },
            { "DOWNLOAD_MAX",              XcpOptionalCommand::DownloadMax },
            { "DOWNLOAD_NEXT",             XcpOptionalCommand::DownloadNext },
            { "FREE_DAQ",                  XcpOptionalCommand::FreeDaq },
            { "GET_CAL_PAGE",              XcpOptionalCommand::GetCalPage },
            { "GET_COMM_MODE_INFO",        XcpOptionalCommand::GetCommModeInfo },
            { "GET_DAQ_CLOCK",             XcpOptionalCommand::GetDaqClock },
            { "GET_DAQ_EVENT_INFO",        XcpOptionalCommand::GetDaqEventInfo },
            { "GET_DAQ_LIST_INFO",         XcpOptionalCommand::GetDaqListInfo },
            { "GET_DAQ_LIST_MODE",         XcpOptionalCommand::GetDaqListMode },
            { "GET_DAQ_PROCESSOR_INFO",    XcpOptionalCommand::GetDaqProcessorInfo },
            { "GET_DAQ_RESOLUTION_INFO",   XcpOptionalCommand::GetDaqResolutionInfo },
            { "GET_ID",                    XcpOptionalCommand::GetId },
            { "GET_PAGE_INFO",             XcpOptionalCommand::GetPageInfo },
            { "GET_PAG_PROCESSOR_INFO",    XcpOptionalCommand::GetPagProcessorInfo },
            { "GET_PGM_PROCESSOR_INFO",    XcpOptionalCommand::GetPgmProcessorInfo },
            { "GET_SECTOR_INFO",           XcpOptionalCommand::GetSectorInfo },
            { "GET_SEED",                  XcpOptionalCommand::GetSeed },
            { "GET_SEGMENT_INFO",          XcpOptionalCommand::GetSegmentInfo },
            { "GET_SEGMENT_MODE",          XcpOptionalCommand::GetSegmentMode },
            { "MODIFY_BITS",               XcpOptionalCommand::ModifyBits },
            { "PROGRAM",                   XcpOptionalCommand::Program },
            { "PROGRAM_CLEAR",             XcpOptionalCommand::ProgramClear },
            { "PROGRAM_FORMAT",            XcpOptionalCommand::ProgramFormat },
            { "PROGRAM_MAX",               XcpOptionalCommand::ProgramMax },
            { "PROGRAM_NEXT",              XcpOptionalCommand::ProgramNext },
            { "PROGRAM_PREPARE",           XcpOptionalCommand::ProgramPrepare },
            { "PROGRAM_RESET",             XcpOptionalCommand::ProgramReset },
            { "PROGRAM_START",             XcpOptionalCommand::ProgramStart },
            { "PROGRAM_VERIFY",            XcpOptionalCommand::ProgramVerify },
            { "READ_DAQ",                  XcpOptionalCommand::ReadDaq },
            { "SET_CAL_PAGE",              XcpOptionalCommand::SetCalPage },
            { "SET_DAQ_LIST_MODE",         XcpOptionalCommand::SetDaqListMode },
            { "SET_DAQ_PTR",               XcpOptionalCommand::SetDaqPtr },
            { "SET_MTA",                   XcpOptionalCommand::SetMta },
            { "SET_REQUEST",               XcpOptionalCommand::SetRequest },
            { "SET_SEGMENT_MODE",          XcpOptionalCommand::SetSegmentMode },
            { "SHORT_DOWNLOAD",            XcpOptionalCommand::ShortDownload },
            { "SHORT_UPLOAD",              XcpOptionalCommand::ShortUpload },
            { "START_STOP_DAQ_LIST",       XcpOptionalCommand::StartStopDaqList },
            { "START_STOP_SYNCH",          XcpOptionalCommand::StartStopSynch },
            { "TRANSPORT_LAYER_CMD",       XcpOptionalCommand::TransportLayerCmd },
            { "UNLOCK",                    XcpOptionalCommand::Unlock },
            { "UPLOAD",                    XcpOptionalCommand::Upload },
            { "USER_CMD",                  XcpOptionalCommand::UserCmd },
            { "WRITE_DAQ",                 XcpOptionalCommand::WriteDaq },
        };

        static inline const std::map<std::string, XcpOverloadIndication> OverloadIndication =
        {
            { "NO_OVERLOAD_INDICATION",   XcpOverloadIndication::None },
            { "OVERLOAD_INDICATION_EVENT", XcpOverloadIndication::Event },
            { "OVERLOAD_INDICATION_PID",   XcpOverloadIndication::Pid },
        };

    private:
        XcpEnumText() = default;
    };

}
