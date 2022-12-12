#include "application.h"

static void application_handle_keyboard_event(
    application_t application,
    const SDL_KeyboardEvent *const keyboard_event);

int application_create(application_t *application, const char **error)
{
    *application = calloc(1, sizeof(struct application_s));

    audio_create(&(*application)->audio);

    graphics_create(&(*application)->graphics);

    return 0;
}

int application_initialize(application_t application, const char *const registry_directory, const char **error)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    audio_initialize(application->audio, registry_directory);

    graphics_initialize(application->graphics);

    application->running = SDL_TRUE;

    return 0;
}

int application_loop(application_t application, const char **error)
{
    SDL_Event event;

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
        if(keyboard_event->keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            application->running = SDL_FALSE;
        }
        else
        {
            application->graphics->rotating = !application->graphics->rotating;
            audio_play_random(application->audio);
        }
    }
}