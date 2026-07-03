find_package(OptiScanCore)
add_library(OptiScanView STATIC)
target_link_libraries(OptiScanView PUBLIC
    OptiScanCore
)

target_include_directories(OptiScanView PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../..)

set(OptiScanView_SOURCES
    Console/ConsoleLogHandler.cpp
    Console/ConsoleLogHandler.h
)

target_sources(OptiScanView PRIVATE ${OptiScanView_SOURCES})
