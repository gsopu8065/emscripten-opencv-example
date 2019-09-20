cmake_minimum_required( VERSION 3.1 )
project( Example1 )

# Use C++ 11 by default
set( CMAKE_CXX_STANDARD 11 )

# Set Release as default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

add_executable( example1 Example1.cpp )

# Specify linker arguments
set_target_properties( example1 PROPERTIES LINK_FLAGS "-s DEMANGLE_SUPPORT=1 -O3 --llvm-lto 1 --closure 1 -s WASM=0 --bind" )