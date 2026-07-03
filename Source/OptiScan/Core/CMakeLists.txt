find_package(Extern)

add_library(OptiScanCore STATIC)

target_link_libraries(OptiScanCore PUBLIC
    Extern
)

target_include_directories(OptiScanCore PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../..)

set(OptiScanCore_SOURCES
    Config/SystemConfig.cpp
    Config/SystemConfig.h
)

target_sources(OptiScanCore PRIVATE ${OptiScanCore_SOURCES})
