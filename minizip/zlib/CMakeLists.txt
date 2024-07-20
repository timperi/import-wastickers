cmake_minimum_required(VERSION 3.14)

set(library_target ZLIB)
project(${library_target} VERSION 0.1 LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(sources
    src/crc32.h
    src/deflate.h
    src/gzguts.h
    src/inffast.h
    src/inffixed.h
    src/inflate.h
    src/inftrees.h
    src/zutil.h

    src/adler32.c
    src/compress.c
    src/crc32.c
    src/deflate.c
    src/gzclose.c
    src/gzlib.c
    src/gzread.c
    src/gzwrite.c
    src/infback.c
    src/inffast.c
    src/inflate.c
    src/inftrees.c
    src/trees.c
    src/uncompr.c
    src/zutil.c
)

set(includes
    include/zlib.h
    include/zconf.h
)

add_library(${library_target} STATIC ${sources} ${includes})

# the libs that this library needs
#target_link_libraries(${library_target} PUBLIC
#)

set_target_properties(${library_target} PROPERTIES LINKER_LANGUAGE C)

TARGET_INCLUDE_DIRECTORIES(${library_target} PUBLIC
    include
)
