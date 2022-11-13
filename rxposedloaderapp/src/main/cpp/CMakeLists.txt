cmake_minimum_required(VERSION 3.18.1)
# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

project("framework")


# configure a header file to pass some of the CMake settings
# to the source code

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h

# add the executable

add_subdirectory(rxposedLoader)

add_library( # Sets the name of the library.
        Loadtest
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        InjectProxySo.cpp)

# in android studio build.gradle
if (${RXDEBUG} STREQUAL OFF)
    message("this is RXDEBUG OFF")
else()
    message("this is RXDEBUG ON")
endif()




message("${RXDEBUG}")

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

target_link_libraries( # Specifies the target library.
        Loadtest
        #        hookbase                         #直接链接就可以，不用进行导入等操作
        # Links the target library to the log library
        # included in the NDK.
        rxposedLoader
        ${log-lib})


