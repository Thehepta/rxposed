cmake_minimum_required(VERSION 3.22.1)
project (ptraceInit)
# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h

# add the executable

set(CMAKE_CXX_STANDARD 17)


add_library( event
        STATIC
        IMPORTED)

set_target_properties( event
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/ptraceInit/libevent/libs/${ANDROID_ABI}/libevent.a)

include_directories("libevent/include")

add_executable(ptraceInit main.cpp InjectProc.cpp logging.cpp)

target_link_libraries( # Specifies the target library.
        ptraceInit
        event
        log)
