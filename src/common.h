#ifndef COMMON_H
#define COMMON_H

#ifdef _WIN32
#include <Windows.h>
//TODO: remove OpenGL
#include <gl/GLU.h>
#include <gl/GL.h>
#else
//TODO: remove opengl/add unix specific
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#endif