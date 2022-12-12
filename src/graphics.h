#ifndef GRAPHICS_H
#define GFRAPHICS_H

#include <GL/glu.h>
#include <GL/gl.h>
#include <SDL3/SDL.h>

struct graphics_s
{
    SDL_Window *window;
    SDL_GLContext gl_context;
    SDL_Thread *render_thread;
    SDL_bool rendering;
};

typedef struct graphics_s *graphics_t;

int graphics_create(graphics_t *graphics);

int graphics_init(graphics_t graphics)
{
    graphics->render_thread = SDL_CreateThread( )
}


#endif