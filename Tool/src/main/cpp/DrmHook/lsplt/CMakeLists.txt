cmake_minimum_required(VERSION 3.4.1)
project(lsplt)


add_definitions(-std=c++20)

set(SOURCES lsplt.cc elf_util.cc)

option(LSPLT_BUILD_SHARED "If ON, lsplt will also build shared library" ON)


add_library(${PROJECT_NAME}_static STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME}_static PUBLIC include)
target_include_directories(${PROJECT_NAME}_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED DEBUG_SYMBOLS_PATH)
    set(DEBUG_SYMBOLS_PATH ${CMAKE_BINARY_DIR}/symbols)
endif()

target_link_libraries(${PROJECT_NAME}_static PUBLIC log)
