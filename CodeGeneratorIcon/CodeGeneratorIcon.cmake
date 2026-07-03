include_guard(GLOBAL)

function(code_generator_tool_apply_version target)
    get_target_property(VERSION_MAJOR ${target} VERSION_MAJOR)
    get_target_property(VERSION_MINOR ${target} VERSION_MINOR)
    get_target_property(VERSION_REVISION ${target} VERSION_REVISION)
    get_target_property(VERSION_ORGANIZATION ${target} VERSION_ORGANIZATION)
    get_target_property(VERSION_PRODUCT ${target} VERSION_PRODUCT)
    get_target_property(VERSION_DESCRIPTION ${target} VERSION_DESCRIPTION)
    get_target_property(VERSION_COPYRIGHT ${target} VERSION_COPYRIGHT)
    get_target_property(VERSION_FILE_NAME ${target} VERSION_FILE_NAME)
    target_compile_definitions(${target} PRIVATE
            "VERSION_MAJOR=${VERSION_MAJOR}"
            "VERSION_MINOR=${VERSION_MINOR}"
            "VERSION_REVISION=${VERSION_REVISION}"
            "VERSION_ORGANIZATION=\"${VERSION_ORGANIZATION}\""
            "VERSION_PRODUCT=\"${VERSION_PRODUCT}\""
            "VERSION_DESCRIPTION=\"${VERSION_DESCRIPTION}\""
            "VERSION_COPYRIGHT=\"${VERSION_COPYRIGHT}\""
            "VERSION_FILE_NAME=\"${VERSION_FILE_NAME}\""
    )
endfunction()

function(code_generator_tool_define_file_name target base)
    if(WIN32)
        set_target_properties(${target} PROPERTIES VERSION_FILE_NAME "${base}.exe")
    else()
        set_target_properties(${target} PROPERTIES VERSION_FILE_NAME "${base}")
    endif()
endfunction()
