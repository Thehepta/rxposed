cmake_minimum_required(VERSION 3.22.1)
project (system_properties)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# configure a header file to pass some of the CMake settings
# to the source code

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h

# add the executable
add_library(system_properties STATIC
        context_node.cpp
        contexts_serialized.cpp
        contexts_split.cpp
        prop_area.cpp
        prop_info.cpp
        system_properties.cpp
        system_property_api.cpp
        system_property_set.cpp
        property_info_parser.cpp
    )
