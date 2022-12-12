#include <SDL3/SDL.h>

#include <stdio.h>

int main(int argc, const char ** argv)
{
    int index;
    for(index = 0; index < argc; index++)
    {
        puts(*(argv + index));
    }

    SDL_Window *window;
    SDL_Event event;
    SDL_bool loop;

    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("Johnny's Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    loop = SDL_TRUE;

    while (loop == SDL_TRUE)
    {
        if (SDL_PollEvent(&event) > 0)
        {
            if (event.type == SDL_QUIT)
            {
                loop = SDL_FALSE;
            }
        }
    }

    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}