cmake_minimum_required(VERSION 3.16)

project(Moeroid)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用MSVC特有扩展

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  execute_process(COMMAND chcp 65001)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Zi")
add_compile_options("/utf-8" "/wd4018" "/wd4244" "/wd4996")
add_link_options("/NODEFAULTLIB:LIBCMT")
if (CMAKE_CL_64)
  add_link_options("/BASE:0x800000000")
endif()

include(cmake/Glad.cmake)
include(cmake/Core.cmake)
include(cmake/Framework.cmake)
include(cmake/Main.cmake)
add_subdirectory(App)