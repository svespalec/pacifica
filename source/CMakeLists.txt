cmake_minimum_required( VERSION 3.21 )

# static linking (must be before project())
set( VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "" )
set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" )

project( pacifica VERSION 1.0.0 LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# output to build/
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build" )

add_subdirectory( cheat )
add_subdirectory( loader )

