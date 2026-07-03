find_package(Extern)

add_library(OptiScanCore STATIC)

target_link_libraries(OptiScanCore PUBLIC
    Extern
)

target_include_directories(OptiScanCore PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../..)

set(OptiScanCore_SOURCES
    Common/Version.cpp
    Common/Version.h
    Config/ConfigDatabase.cpp
    Config/ConfigDatabase.h
    Config/SystemConfig.cpp
    Config/SystemConfig.h
    Config/SystemConfigConstants.h
    Json/JsonSchemaValidator.h
    Json/JsonSchemaValidator.cpp
    LogHandler.h
)

target_sources(OptiScanCore PRIVATE ${OptiScanCore_SOURCES})
