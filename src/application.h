#ifndef APPLICATION_H
#define APPLICATION_H

#include "audio.h"
#include "graphics.h"

struct application_s
{
    SDL_Thread *audio_thread;
    SDL_Thread *graphics_thread;
};
typedef struct application_s *application_t;

int application_create(application_t *application, const char ** error);

int application_initialize(application_t application, const char * const registry_path, const char ** error);

int application_loop(application_t application, const char ** error);

void application_destroy(application_t application);

#endif