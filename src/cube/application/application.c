#include "cube.h"

static void application_handle_keyboard_event(
    cube_application *application,
    const SDL_KeyboardEvent *const keyboard_event);

int application_create(cube_application **application, const char *resource_directory)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(application != NULL, "invalid application handle")

    CUBE_ASSERT(SDL_Init(SDL_INIT_EVERYTHING) >= 0, SDL_GetError())

    *application = calloc(1, sizeof(cube_application));
    CUBE_ASSERT(*application != NULL, "failed to allocate application")

    CUBE_ASSERT(
        graphics_create(
            &(*application)->graphics, resource_directory) == CUBE_SUCCESS,
        "failed to create graphics subsystem")
    CUBE_END_FUNCTION
}

int application_loop(cube_application *application)
{
    CUBE_BEGIN_FUNCTION
    SDL_Event event;
    application->loop = SDL_TRUE;
    while (application->loop == SDL_TRUE)
    {
        CUBE_ASSERT(
            graphics_render(
                application->graphics) == CUBE_SUCCESS,
            "render error")
        if (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                application->loop = SDL_FALSE;
                break;
            case SDL_KEYDOWN:
                application_handle_keyboard_event(
                    application,
                    (SDL_KeyboardEvent *)&event);
                break;
            }
        }
    }
    CUBE_END_FUNCTION
}

void application_destroy(cube_application *application)
{
    graphics_destroy(application->graphics);
    free(application);
    SDL_Quit();
}

void application_handle_keyboard_event(
    cube_application *application,
    const SDL_KeyboardEvent *const keyboard_event)
{
    if (keyboard_event->repeat == 0)
    {
        if (keyboard_event->keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            application->loop = SDL_FALSE;
        }
        // TODO: handle else
    }
}