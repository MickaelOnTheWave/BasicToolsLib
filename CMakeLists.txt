cmake_minimum_required (VERSION 3.2)

project (BasicToolsLib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(projectRoot "${CMAKE_CURRENT_SOURCE_DIR}")
set(srcDir "${projectRoot}/src")
set(includeDir "${projectRoot}/include")

file(GLOB sources "${srcDir}/*")
file(GLOB includes "${includeDir}/*")

add_library(BasicToolsLib ${sources})

add_subdirectory(tests)

target_include_directories(BasicToolsLib PUBLIC "${includeDir}")

