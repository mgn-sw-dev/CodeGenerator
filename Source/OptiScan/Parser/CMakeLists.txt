find_package(Extern)

add_library(OptiScanParser STATIC)

target_link_libraries(OptiScanParser PUBLIC
    Extern
)

target_include_directories(OptiScanParser PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../..)

set(OptiScanParser_SOURCES
    A2l/A2lDatabase.h
    A2l/A2lKeyword.h
    A2l/A2lParser.cpp
    A2l/A2lParser.h
    A2l/A2lScanner.cpp
    A2l/A2lScanner.h
    A2l/A2lTokenReader.cpp
    A2l/A2lTokenReader.h
    A2l/McdIdent.cpp
    A2l/McdIdent.h
    A2l/McdModule.cpp
    A2l/McdModule.h
    Dbc/DbcAttribute.h
    Dbc/DbcBitTiming.cpp
    Dbc/DbcBitTiming.h
    Dbc/DbcComment.h
    Dbc/DbcDatabase.h
    Dbc/DbcException.cpp
    Dbc/DbcException.h
    Dbc/DbcFormat.cpp
    Dbc/DbcFormat.h
    Dbc/DbcKeyword.h
    Dbc/DbcMessage.cpp
    Dbc/DbcMessage.h
    Dbc/DbcParser.cpp
    Dbc/DbcParser.h
    Dbc/DbcScanner.cpp
    Dbc/DbcScanner.h
    Dbc/DbcSignal.h
    Dbc/DbcTokenReader.cpp
    Dbc/DbcTokenReader.h
    Dbc/DbcValue.h
    Ldf/LdfDatabase.h
    Ldf/LdfKeyword.h
    Ldf/LdfModes.h
    Ldf/LdfParser.cpp
    Ldf/LdfParser.h
    Ldf/LdfScanner.cpp
    Ldf/LdfScanner.h
    Ldf/LdfTokenReader.cpp
    Ldf/LdfTokenReader.h
    Ldf/LinSignal.h
    Ldf/LinSignalEncodingType.h
    CharReader.cpp
    CharReader.h
    FileException.cpp
    FileException.h
    Flags.h
    Flags.inl.h
    ScanPosition.h
    Token.h
    TokenReaderUtils.cpp
    TokenReaderUtils.h
)

target_sources(OptiScanParser PRIVATE ${OptiScanParser_SOURCES})
