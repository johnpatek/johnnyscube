#ifndef APPLICATION_H
#define APPLICATION_H

#include "audio.h"
#include "graphics.h"

struct application_s
{
    graphics_t graphics;
    audio_t audio;
    SDL_bool running;
};
typedef struct application_s *application_t;

int application_create(
    application_t *application, 
    const char* const resource_directory);

int application_loop(application_t application);

void application_destroy(application_t application);

#endif