#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GL/glu.h>
#include <GL/gl.h>
#include <SDL3/SDL.h>

struct graphics_s
{
    SDL_DisplayMode display_mode;
    SDL_Window *window;
    SDL_GLContext gl_context;
    SDL_Thread *render_thread;
    SDL_bool rendering;
    SDL_bool rotating;
};
typedef struct graphics_s *graphics_t;

int graphics_create(graphics_t *graphics);

int graphics_initialize(graphics_t graphics);

int graphics_render(graphics_t graphics);

void graphics_destroy(graphics_t graphics);

#endif