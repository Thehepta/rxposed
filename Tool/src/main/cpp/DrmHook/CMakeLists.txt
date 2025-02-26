# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("DrmHook")


add_subdirectory(lsplt)

add_library( # Sets the name of the library.
        DrmHook
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        DrmHook.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries( # Specifies the target library.
        DrmHook
        lsplt_static
#        rxposed
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

