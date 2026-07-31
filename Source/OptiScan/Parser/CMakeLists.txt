find_package(Extern)

add_library(OptiScanParser STATIC)

target_link_libraries(OptiScanParser PUBLIC
    Extern
)

target_include_directories(OptiScanParser PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../..)

set(OptiScanParser_SOURCES
    Dbc/DbcAttribute.h
    Dbc/DbcComment.h
    Dbc/DbcDatabase.cpp
    Dbc/DbcDatabase.h
    Dbc/DbcException.cpp
    Dbc/DbcException.h
    Dbc/DbcFormat.cpp
    Dbc/DbcFormat.h
    Dbc/DbcKeyword.h
    Dbc/DbcParser.cpp
    Dbc/DbcParser.h
    Dbc/DbcScanner.cpp
    Dbc/DbcScanner.h
    Flags.h
    Flags.inl.h
)

target_sources(OptiScanParser PRIVATE ${OptiScanParser_SOURCES})
