cmake_minimum_required(VERSION 4.3.1)

project(cpp-ps1 VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "" FORCE)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "CCache found: ${CCACHE_PROGRAM}")

    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
else()
    message(STATUS "CCache not found")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(
        -O3
        -ffast-math
        -march=native 
        -flto=full
        -fno-exceptions 
        -fno-rtti 
        -pipe 
    )


    add_link_options(
        -flto=full
        -fuse-ld=lld
        -Wl,--icf=all
        -Wl,--gc-sections
        -Wl,-O3
        -Wl,--threads=8
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
        -O0
        -pg
    )
endif()

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(cpp-ps1 ${SRC_FILES} ${HEADER_FILES})

target_include_directories(cpp-ps1 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)