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
    Config/ConfigDatabaseBus.cpp
    Config/ConfigDatabaseBus.h
    Config/ConfigDatabaseCan.cpp
    Config/ConfigDatabaseCan.h
    Config/ConfigDatabaseLin.cpp
    Config/ConfigDatabaseLin.h
    Config/ConfigDatabaseStandardTrace.cpp
    Config/ConfigDatabaseStandardTrace.h
    Config/ConfigDatabaseTrigger.cpp
    Config/ConfigDatabaseTrigger.h
    Database/CanDatabase.cpp
    Database/CanDatabase.h
    Database/DbcDatabase.cpp
    Database/DbcDatabase.h
    Database/SelectedSignal.cpp
    Database/SelectedSignal.h
    Database/SelectionTable.cpp
    Database/SelectionTable.h
    Database/SelectionTableConstants.h
    Database/SystemDatabase.cpp
    Database/SystemDatabase.h
    Database/SystemMapping.cpp
    Database/SystemMapping.h
    Config/SystemConfig.cpp
    Config/SystemConfig.h
    Config/SystemConfig.inl.h
    Config/SystemConfigConstants.h
    Json/JsonFile.cpp
    Json/JsonFile.h
    Json/JsonSchemaValidator.h
    Json/JsonSchemaValidator.cpp
    LogHandler.cpp
    LogHandler.h
)

target_sources(OptiScanCore PRIVATE ${OptiScanCore_SOURCES})
