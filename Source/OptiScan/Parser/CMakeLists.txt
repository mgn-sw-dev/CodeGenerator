find_package(Extern)

add_library(OptiScanParser STATIC)

target_link_libraries(OptiScanParser PUBLIC
    Extern
)

target_include_directories(OptiScanParser PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../..)

set(OptiScanParser_SOURCES
    Dbc/DbcDatabase.cpp
    Dbc/DbcDatabase.h
    Dbc/DbcParser.cpp
    Dbc/DbcParser.h
    Dbc/DbcScanner.cpp
    Dbc/DbcScanner.h
)

target_sources(OptiScanParser PRIVATE ${OptiScanParser_SOURCES})
