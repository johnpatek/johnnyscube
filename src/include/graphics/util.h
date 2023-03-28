#ifndef CUBE_GRAPHICS_UTIL_H
#define CUBE_GRAPHICS_UTIL_H

#include "types.h"

int graphics_util_load_shader(
    cube_graphics *graphics,
    const char *shader_file,
    VkShaderModule *shader_module);

int graphics_util_upload_buffer(
    cube_graphics *graphics,
    VkBufferUsageFlags usage,
    const void *data,
    size_t size,
    VkBuffer *buffer,
    VmaAllocation *buffer_allocation);

int graphics_util_rotate(
    float model[4][4], 
    float angle, 
    float axis[3], 
    cube_ubo *ubo);

int graphics_util_look_at(
    float eye[3],
    float center[3], 
    float up[3], 
    cube_ubo *ubo);

int graphics_util_perspective(
    float fov,
    float aspect, 
    float znear, 
    float zfar,
    cube_ubo *ubo);

#endif