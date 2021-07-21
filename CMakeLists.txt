cmake_minimum_required (VERSION 3.0.0)

project (token_index VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

aux_source_directory(src/neu NEU)

add_executable (test1 tests/test1.cpp
    ${NEU}
)

add_executable (test2 tests/test2.cpp
)