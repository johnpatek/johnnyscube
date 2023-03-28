#ifndef CUBE_GRAPHICS_H
#define CUBE_GRAPHICS_H

#include "graphics/display.h"
#include "graphics/device.h"
#include "graphics/object.h"
#include "graphics/pipeline.h"
#include "graphics/frame.h"
#include "graphics/util.h"

int graphics_create(
    cube_graphics **graphics, 
    const char * resource_directory);

int graphics_render(cube_graphics *graphics);

void graphics_destroy(cube_graphics *graphics);

#endif