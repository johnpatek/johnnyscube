file(GLOB CUBE_SOURCES *.c *.h)

add_executable(cube ${CUBE_SOURCES})

if(WIN32)
target_include_directories(cube PRIVATE ${DIRENT_INCLUDE} ${CMAKE_SOURCE_DIR}/SDL/include Vulkan::Headers ${CMAKE_SOURCE_DIR}/SDL/src/video/khronos)
target_link_libraries(cube SDL3-static SDL3_main Vulkan::Vulkan)
else()
target_include_directories(cube PRIVATE ${CMAKE_SOURCE_DIR}/SDL/include ${CMAKE_SOURCE_DIR}/SDL/src/video/khronos)

target_link_libraries(cube SDL3 /usr/lib/x86_64-linux-gnu/libvulkan.so.1)
endif()