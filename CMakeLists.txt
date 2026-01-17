cmake_minimum_required(VERSION 3.16)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

set(TARGET_NAME "subins")

project(${TARGET_NAME}
    DESCRIPTION ${TARGET_NAME}
    LANGUAGES C CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# output name
add_executable(${TARGET_NAME})

# complier and linker flags

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

include(CheckIncludeFileCXX)
check_include_file_cxx(format HAVE_STDFORMAT_HEADER CMAKE_REQUIRED_QUIET)

if(!HAVE_STDFORMAT_HEADER)
  target_include_directories(${TARGET_NAME} PUBLIC polyfill)
endif()

set(HEADERS
./build_instructions.h
./post_processing.h
)

set(SOURCES
./post_processing.cpp
./sh_insns.cpp
./build_instructions.cpp
)


target_sources(${TARGET_NAME} PUBLIC
  ${HEADERS} ${SOURCES}
)
