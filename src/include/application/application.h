#ifndef CUBE_APPLICATION_H
#define CUBE_APPLICATION_H

#include "audio.h"
#include "graphics.h"

typedef struct _cube_application
{
    SDL_bool loop;
    cube_graphics *graphics;
} cube_application;

int application_create(cube_application **application, const char * resource_directory);

int application_loop(cube_application *application);

void application_destroy(cube_application *application);

#endif