cmake_minimum_required(VERSION 2.8)

project(kluster)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

set(out_dir ${CMAKE_BINARY_DIR}/output)
set(external_dir ${CMAKE_BINARY_DIR}/external)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${out_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${out_dir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${out_dir})

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )

find_package(ZLIB REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ROOT "../external/boost")

find_package(Boost 1.50.0
              COMPONENTS
              system
              filesystem
              thread
              program_options
              iostreams
              serialization
              wserialization
            REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories("include")

add_subdirectory(external)
add_subdirectory(messages)
add_subdirectory(transport)
add_subdirectory(kluster_client)
add_subdirectory(kluster_broker)
add_subdirectory(kluster_worker)
add_subdirectory(test)
