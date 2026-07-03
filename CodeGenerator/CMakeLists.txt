cmake_minimum_required(VERSION 3.19)
project(CodeGenerator)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/Desktop.cmake)

# ToDo: Qt only for View needed
qt_standard_project_setup()
add_subdirectory("../Source" "Source")

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/_PACKAGE/CodeGenerator
        #	PARENT_SCOPE
)
install(CODE "file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX})")
install(TARGETS
        CodeGenerator
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}
)

if(WIN32 AND CMAKE_HOST_WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT
            NAMES windeployqt
            HINTS "${QT_BIN_DIR}"
            REQUIRED
    )

    string(APPEND command "execute_process(COMMAND \"${WINDEPLOYQT}\"")
    string(APPEND command " --translations en")
    #string(APPEND command " --verbose 2")
    string(APPEND command " \"\${CMAKE_INSTALL_PREFIX}/CodeGenerator.exe\"")
    string(APPEND command ")")
    install(CODE ${command})

    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        install(CODE "file(REMOVE \"\${CMAKE_INSTALL_PREFIX}/vc_redist.x86.exe\")")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        install(CODE "file(REMOVE \"\${CMAKE_INSTALL_PREFIX}/vc_redist.x64.exe\")")
    endif()
endif()
