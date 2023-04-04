#include "cube.h"

int graphics_create(
    cube_graphics **graphics,
    const char *resource_directory)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(graphics != NULL, "NULL graphics handle")

    *graphics = calloc(1, sizeof(cube_graphics));
    CUBE_ASSERT(*graphics != NULL, "failed to allocate graphics")

    SDL_asprintf(&(*graphics)->shader_directory, "%s%s%s", resource_directory, PATH_SEPARATOR, "shaders");
    CUBE_PUSH((*graphics)->shader_directory);

    CUBE_ASSERT(graphics_create_display(*graphics) == CUBE_SUCCESS, "failed to create display")
    CUBE_ASSERT(graphics_create_device(*graphics) == CUBE_SUCCESS, "failed to create device")
    CUBE_ASSERT(graphics_create_object(*graphics) == CUBE_SUCCESS, "failed to create object")
    CUBE_ASSERT(graphics_create_images(*graphics) == CUBE_SUCCESS, "failed to create images")
    CUBE_ASSERT(graphics_create_pipeline(*graphics) == CUBE_SUCCESS, "failed to create pipeline")
    CUBE_ASSERT(graphics_create_frame_pool(*graphics) == CUBE_SUCCESS, "failed to create frame pool")
    CUBE_END_FUNCTION
}

int graphics_render(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    cube_frame *frame;

    CUBE_ASSERT(
        graphics_render_acquire_frame(
            graphics, &frame) == CUBE_SUCCESS,
        "failed to acquire frame")

    CUBE_ASSERT(
        graphics_render_draw_frame(
            graphics,
            frame) == CUBE_SUCCESS,
        "failed to draw frame")

    CUBE_ASSERT(
        graphics_render_submit_frame(
            graphics,
            frame) == CUBE_SUCCESS,
        "failed to submit frame")

    CUBE_END_FUNCTION
}

void graphics_destroy(cube_graphics *graphics)
{
    if (graphics != NULL)
    {
        graphics_destroy_frame_pool(graphics);
        graphics_destroy_images(graphics);
        graphics_destroy_pipeline(graphics);
        graphics_destroy_object(graphics);
        graphics_destroy_device(graphics);
        graphics_destroy_display(graphics);
        free(graphics);
    }
}
