find_package(CodeGeneratorIcon)
find_package(OptiScanCore)
find_package(OptiScanView)
# ToDo: Qt only for View needed
qt_add_executable(CodeGenerator)

target_link_libraries(CodeGenerator PRIVATE
        OptiScanCore
        OptiScanView
        CodeGeneratorIcon
        # OptiScanViews
)
set_target_properties(CodeGenerator PROPERTIES
        VERSION_MAJOR 0
        VERSION_MINOR 0
        VERSION_REVISION 0
        VERSION_ORGANIZATION "Estino"
        VERSION_PRODUCT "Code Generator Gen2"
        VERSION_DESCRIPTION "Estino Code Generator Gen2"
        VERSION_COPYRIGHT "(C) 2026"
)
code_generator_tool_define_file_name(CodeGenerator "CodeGenerator")
code_generator_tool_apply_version(CodeGenerator)

target_include_directories(CodeGenerator PRIVATE .)

set(CodeGenerator_SOURCES
        main.cpp
)

target_sources(CodeGenerator PRIVATE ${CodeGenerator_SOURCES})
