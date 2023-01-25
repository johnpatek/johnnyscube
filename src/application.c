#include "application.h"

static void application_handle_keyboard_event(
    application_t application,
    const SDL_KeyboardEvent *const keyboard_event);

int application_create(
    application_t *application,
    const char *const resource_directory)
{
    int status;
    application_t new_application;

    status = CUBE_SUCCESS;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr,"application_create: failed to initialize SDL(%s)\n", SDL_GetError());
        goto error;
    }

    new_application = calloc(1, sizeof(struct application_s));
    if (new_application == NULL)
    {
        fprintf(stderr, "application_create: failed to allocate new application\n");
        goto error;
    }

    if (audio_create(&(new_application)->audio, resource_directory) != CUBE_SUCCESS)
    {
        fprintf(stderr, "application_create: failed to create audio subsystem\n");
        goto error;
    }

    if (graphics_create(&(new_application)->graphics, resource_directory) != CUBE_SUCCESS)
    {
        fprintf(stderr, "application_create: failed to create graphics subsystem\n");
        goto error;
    }

    *application = new_application;

    goto done;
error:
    status = CUBE_FAILURE;
    application_destroy(new_application);
done:
    return 0;
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