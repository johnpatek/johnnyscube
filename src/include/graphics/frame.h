#ifndef CUBE_GRAPHICS_FRAME_H
#define CUBE_GRAPHICS_FRAME_H

#include "types.h"

int graphics_create_swapchain(cube_graphics *graphics);

int graphics_create_frame_pool(cube_graphics *graphics);

int graphics_render_acquire_frame(cube_graphics *graphics, cube_frame **frame);

int graphics_render_draw_frame(cube_graphics *graphics, cube_frame *frame);

int graphics_render_submit_frame(cube_graphics *graphics, cube_frame *frame);

void graphics_destroy_swapchain(cube_graphics *graphics);

void graphics_destroy_frame_pool(cube_graphics *graphics);

#endif