add_library(Extern INTERFACE)
target_include_directories(Extern INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Expat is the only external dependency that is not header only. Its own CMake build
# is used instead of a hand maintained source list: the file layout changed noticeably
# between 2.6 and 2.10 (xcs.c, entities.h, hash_table.h and one random_*.c per
# platform), so a hand written list would silently break on the next update.
#
# Everything but the static library itself is switched off, so no tool, test, example,
# man page or install rule reaches the CodeGenerator build.
set(EXPAT_BUILD_TOOLS      OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_TESTS      OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_DOCS       OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_FUZZERS    OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_PKGCONFIG  OFF CACHE BOOL "" FORCE)
set(EXPAT_SHARED_LIBS      OFF CACHE BOOL "" FORCE)
set(EXPAT_ENABLE_INSTALL   OFF CACHE BOOL "" FORCE)

add_subdirectory(Expat EXCLUDE_FROM_ALL)

# Hide warnings of the vendored C sources; they are not ours to fix.
if(MSVC)
    target_compile_options(expat PRIVATE /W0)
else()
    target_compile_options(expat PRIVATE -w)
endif()

target_link_libraries(Extern INTERFACE expat::expat)
