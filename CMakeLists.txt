cmake_minimum_required(VERSION 3.21)
project(FramePerfectDetector)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (NOT DEFINED GEODE_SDK)
    message(FATAL_ERROR "GEODE_SDK not defined. Make sure to run CMake with -DGEODE_SDK=<path>")
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

include(${GEODE_SDK}/cmake/geode.cmake)
