cmake_minimum_required(VERSION 3.0)

project(johnnyscube)

find_package(Vulkan)

if(WIN32)
    set(DIRENT_INCLUDE ${CMAKE_SOURCE_DIR}/dirent/include)
endif()

add_subdirectory(SDL)
add_subdirectory(VulkanMemoryAllocator)

file(
    GLOB 
    CUBE_SOURCES 
    ${CMAKE_SOURCE_DIR}/src/cube/main.c 
    ${CMAKE_SOURCE_DIR}/src/cube/application/*.c 
    ${CMAKE_SOURCE_DIR}/src/cube/graphics/*.c 
    ${CMAKE_SOURCE_DIR}/src/cube/audio/*.c) 

add_executable(cube ${CUBE_SOURCES})


target_include_directories(
    cube 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/include 
    ${CMAKE_SOURCE_DIR}/VulkanMemoryAllocator/include 
    ${DIRENT_INCLUDE} 
    ${CMAKE_SOURCE_DIR}/SDL/include Vulkan::Headers 
    ${CMAKE_SOURCE_DIR}/SDL/src/video/khronos)

if(WIN32)
    target_link_libraries(cube VulkanMemoryAllocator SDL3-static SDL3_main Vulkan::Vulkan)
else()
    target_link_libraries(cube VulkanMemoryAllocator SDL3 /usr/lib/x86_64-linux-gnu/libvulkan.so.1 m)
endif()