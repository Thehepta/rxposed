cmake_minimum_required(VERSION 3.22.1)
project (resetprop)
# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")


add_subdirectory(system_properties)
# configure a header file to pass some of the CMake settings
# to the source code

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h

# add the executable
add_executable(resetprop resetprop.cpp)
target_link_libraries(resetprop system_properties log)

