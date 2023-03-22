#include "cube.h"

static int graphics_create_physical_device(cube_graphics *graphics);
static int graphics_create_queue_families(cube_graphics *graphics);
static int graphics_create_logical_device(cube_graphics *graphics);
static int graphics_create_command_pool(cube_graphics *graphics);

int graphics_create_device(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION

    CUBE_END_FUNCTION
}

void graphics_destroy_device(cube_graphics *graphics)
{
    vkDestroyCommandPool(
        graphics->logical_device, 
        NULL, 
        graphics->command_pool);
    vkDestroyDevice(graphics->logical_device, NULL);
}