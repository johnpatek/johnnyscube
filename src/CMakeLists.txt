add_executable(cube main.c application.c application.h audio.c audio.h graphics.c graphics.h)

if(WIN32)
target_include_directories(cube PRIVATE ${CMAKE_SOURCE_DIR}/SDL/include "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.19041.0\\um" ${CMAKE_SOURCE_DIR}/SDL/src/video/khronos)
target_link_directories(cube PRIVATE "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.19041.0\\um\\x64" "C:\\VulkanSDK\\1.3.231.1\\Lib")
target_link_libraries(cube SDL3-static SDL3_main vulkan-1)
else()
target_include_directories(cube PRIVATE ${CMAKE_SOURCE_DIR}/SDL/include ${CMAKE_SOURCE_DIR}/SDL/src/video/khronos)

target_link_libraries(cube SDL3 /usr/lib/x86_64-linux-gnu/libvulkan.so.1)
endif()