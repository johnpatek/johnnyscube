#include "application.h"

static void application_handle_keyboard_event(
    application_t application,
    const SDL_KeyboardEvent *const keyboard_event);

int application_create(
    application_t *application,
    const char *const resource_directory)
{
    CUBE_BEGIN_FUNCTION

    CUBE_ASSERT(SDL_Init(SDL_INIT_EVERYTHING) >= 0, SDL_GetError())

    *application = calloc(1, sizeof(struct application_s));
    CUBE_ASSERT(*application != NULL, "failed to allocate new application")

    CUBE_ASSERT(audio_create(&(*application)->audio, resource_directory) == CUBE_SUCCESS, "failed to create audio subsystem")
    CUBE_ASSERT(graphics_create(&(*application)->graphics, resource_directory) == CUBE_SUCCESS, "failed to create graphics subsystem")

    CUBE_END_FUNCTION
}

int application_loop(application_t application)
{
    SDL_Event event;

    application->running = SDL_TRUE;

    while (application->running == SDL_TRUE)
    {
        graphics_render(application->graphics);
        if (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                application->running = SDL_FALSE;
                break;
            case SDL_KEYDOWN:
                application_handle_keyboard_event(
                    application,
                    (SDL_KeyboardEvent *)&event);
                break;
            }
        }
    }

    return 0;
}

void application_destroy(application_t application)
{
    if (application != NULL)
    {
        audio_destroy(application->audio);
        graphics_destroy(application->graphics);
        free(application);
    }
    SDL_Quit();
}

static void application_handle_keyboard_event(
    application_t application,
    const SDL_KeyboardEvent *const keyboard_event)
{
    if (keyboard_event->repeat == 0)
    {
        if (keyboard_event->keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            application->running = SDL_FALSE;
        }
        else
        {
            audio_play_random(application->audio);
        }
    }
}