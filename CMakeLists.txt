cmake_minimum_required(VERSION 2.6)
project(qr_code_generator)

include_directories(./)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FALGS} -O0 -g -std=c++20 -pthread")

add_executable(qr_code_generator 
    util/data_placer.cpp
    util/version_table.cpp
    generators/data_generator.cpp
    generators/marker_generator.cpp
    generators/mask_generator.cpp
    generators/version_generator.cpp
    generators/print_generator.cpp
    main.cpp 
)
