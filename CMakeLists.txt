
set(ZLIB_USE_STATIC_LIBS "ON")
cmake_minimum_required(VERSION 3.5.0)
project(PNGParser VERSION 0.1.0 LANGUAGES C)

add_subdirectory(Vendor/Vector)

set(ZLIB_PUBLIC_HDRS
    Vendor/zlib-1.3.1/zconf.h.in
    Vendor/zlib-1.3.1/zlib.h
)
set(ZLIB_PRIVATE_HDRS
    Vendor/zlib-1.3.1/crc32.h
    Vendor/zlib-1.3.1/deflate.h
    Vendor/zlib-1.3.1/gzguts.h
    Vendor/zlib-1.3.1/inffast.h
    Vendor/zlib-1.3.1/inffixed.h
    Vendor/zlib-1.3.1/inflate.h
    Vendor/zlib-1.3.1/inftrees.h
    Vendor/zlib-1.3.1/trees.h
    Vendor/zlib-1.3.1/zutil.h
)
set(ZLIB_SRCS
    Vendor/zlib-1.3.1/adler32.c
    Vendor/zlib-1.3.1/compress.c
    Vendor/zlib-1.3.1/crc32.c
    Vendor/zlib-1.3.1/deflate.c
    Vendor/zlib-1.3.1/gzclose.c
    Vendor/zlib-1.3.1/gzlib.c
    Vendor/zlib-1.3.1/gzread.c
    Vendor/zlib-1.3.1/gzwrite.c
    Vendor/zlib-1.3.1/inflate.c
    Vendor/zlib-1.3.1/infback.c
    Vendor/zlib-1.3.1/inftrees.c
    Vendor/zlib-1.3.1/inffast.c
    Vendor/zlib-1.3.1/trees.c
    Vendor/zlib-1.3.1/uncompr.c
    Vendor/zlib-1.3.1/zutil.c
)

add_compile_options(-g)
add_compile_options(-g)

add_library(zlib_png STATIC ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
target_include_directories(zlib_png PRIVATE Vendor/zlib-1.3.1)

add_library(PNGParserLib STATIC ./src/PNGParser.c)

target_include_directories(PNGParserLib PUBLIC Vendor/CRC)
target_include_directories(PNGParserLib PUBLIC Vendor/zlib-1.3.1)
target_include_directories(PNGParserLib PUBLIC Vendor/Vector)
target_include_directories(PNGParserLib PUBLIC Vendor/stb)

target_link_libraries(PNGParserLib PUBLIC zlib_png Vector)

add_executable(PNGParser_Example ./example/main.c)
target_include_directories(PNGParser_Example PRIVATE src/)
target_link_libraries(PNGParser_Example PNGParserLib)
