# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# 设置目标导入路径，这里是一行

message("cmake project name is  ${CMAKE_PROJECT_NAME}")

#if (${RXDEBUG} STREQUAL OFF)
#    message("this is RXDEBUG OFF")
#else()
#    message("this is RXDEBUG ON")
#endif()



add_library(libdobby STATIC IMPORTED)
set_target_properties(libdobby PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/libdobby.a)


#include_directories(${CMAKE_SOURCE_DIR}/include)



add_library( # Sets the name of the library.
        nativeloader
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        zygote_hook.cpp
        InjectApp.cpp
        tool.cpp
        elf_symbol_resolver.cpp
        And64InlineHook.cpp
        artmethod_native_hook.cpp
        rprocess.cpp
        check.cpp
        FunHook.cpp
        service.cpp
        android_shm.cpp
        )



# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        nativeloader
        # Links the target library to the log library
        # included in the NDK.
        libdobby
        ${log-lib} )