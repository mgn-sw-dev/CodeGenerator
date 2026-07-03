find_package(Qt6 REQUIRED COMPONENTS Core)
# ToDo: Qt only for View needed
qt_add_library(CodeGeneratorIcon INTERFACE)
set(CodeGeneratorIcon_SOURCES
        EstinoLogo.ico
)
if(WIN32)
    list(APPEND CodeGeneratorIcon_SOURCES
            CodeGeneratorIcon.rc
    )
endif()
target_include_directories(CodeGeneratorIcon INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_sources(CodeGeneratorIcon INTERFACE ${CodeGeneratorIcon_SOURCES})
