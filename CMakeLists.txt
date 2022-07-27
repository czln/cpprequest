#   available variables:
#   1. CPPREQUEST_LIBRARY

cmake_minimum_required(VERSION 3.16)
project(cpprequest)

set(CMAKE_CXX_STANDARD 14)

find_package(CURL REQUIRED)

include_directories(include)

option(BUILD_SHARED_LIB "enable to build shared library" ON)
message(STATUS "build shared: ${BUILD_SHARED_LIB}")

option(BUILD_EXAMPLE "enable to build example code" ON)
message(STATUS "build examples: ${BUILD_EXAMPLE}")

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
## static library output path
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
## shared library output path
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)



aux_source_directory(src SRC)
if (BUILD_SHARED_LIB)
    add_library(${PROJECT_NAME} SHARED ${SRC})
else()
    add_library(${PROJECT_NAME} ${SRC})
endif()
if (BUILD_EXAMPLE)
    add_subdirectory(examples)
endif ()

if (NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
endif ()

install(TARGETS
        ARCHIVE
        LIBRARY
        PUBLIC_HEADER
)