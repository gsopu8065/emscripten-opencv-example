cmake_minimum_required( VERSION 3.1 )
project( MyOpenCV )

# Use C++ 11 by default
set( CMAKE_CXX_STANDARD 11 )

# Set Release as default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

# Needed for opencv2/opencv.hpp
include_directories( /Users/srujan.gopu/Documents/Opencv/opencv/include )

# Needed by opencv.hpp for opencv2/opencv_modules.hpp
include_directories( /Users/srujan.gopu/Documents/Opencv/opencv/asurion_opencv )

# Needed by opencv_modules.hpp for every module
file( GLOB opencv_include_modules "/Users/srujan.gopu/Documents/Opencv/opencv/modules/*/include" )
include_directories( ${opencv_include_modules} )

add_executable( myopencv MyOpenCV.cpp )

# Link to opencv.js precompiled libraries
file( GLOB opencv_core "/Users/srujan.gopu/Documents/Opencv/opencv/asurion_opencv/lib/*.a" )
target_link_libraries( myopencv ${opencv_core} )

# Specify linker arguments
set_target_properties( myopencv PROPERTIES LINK_FLAGS "-s DEMANGLE_SUPPORT=1 -s ALLOW_MEMORY_GROWTH=1 -Oz -O3 -s WASM_OBJECT_FILES=0 --llvm-lto 1 --closure 1 -s WASM=1 --bind" )