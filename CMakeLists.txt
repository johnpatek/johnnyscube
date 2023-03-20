cmake_minimum_required(VERSION 3.0)

project(johnnyscube)

find_package(Vulkan)

if(WIN32)
    set(DIRENT_INCLUDE ${CMAKE_SOURCE_DIR}/dirent/include)
endif()

add_subdirectory(SDL)

add_subdirectory(src)