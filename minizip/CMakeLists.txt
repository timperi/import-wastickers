cmake_minimum_required(VERSION 3.14)

set(library_target minizip)
project(${library_target} VERSION 0.1 LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(sources
    src/crypt.h
    src/ioapi.c
    src/ioapi_buf.c
    src/ioapi_buf.h
    src/ioapi_mem.c
    src/ioapi_mem.h
    src/iowin32.c
    src/iowin32.h
    src/unzip.c
    src/zip.c
)

set(includes
    include/unzip.h
    include/zip.h
    include/ioapi.h
)

add_library(${library_target} STATIC ${sources} ${includes})

add_subdirectory(zlib)
target_link_libraries(${library_target} PUBLIC ZLIB)

set_target_properties(${library_target} PROPERTIES LINKER_LANGUAGE C)

TARGET_INCLUDE_DIRECTORIES(${library_target} PUBLIC
    include
)
