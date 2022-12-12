#ifndef COMMON_H
#define COMMON_H
#ifdef _WIN32
#include <Windows.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include <SDL3/SDL.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#endif