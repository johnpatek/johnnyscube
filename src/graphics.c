#include "application.h"

int graphics_create(graphics_t *graphics)
{
    *graphics = calloc(1, sizeof(struct graphics_s));
}

int graphics_initialize(graphics_t graphics)
{
    SDL_GetCurrentDisplayMode(0, &graphics->display_mode);

    graphics->window = SDL_CreateWindow(
        "Johnny's Cube",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        graphics->display_mode.w,
        graphics->display_mode.h,
        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

    graphics->gl_context = SDL_GL_CreateContext(
        graphics->window);

    /* Our shading model--Gouraud (smooth). */
    glShadeModel(GL_SMOOTH);

    /* Culling. */
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glClearColor(0, 0, 0, 0);

    glViewport(
        0, 0,
        graphics->display_mode.w,
        graphics->display_mode.h);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(
        60.0,
        (GLdouble)graphics->display_mode.w / (GLdouble)graphics->display_mode.h,
        1.0,
        1024.0);

    glClear(GL_COLOR_BUFFER_BIT);

    graphics->rotating = SDL_FALSE;

    graphics->rendering = SDL_TRUE;

    return 0;
}

void graphics_destroy(graphics_t graphics)
{
    if (graphics != NULL)
    {
        SDL_GL_DeleteContext(graphics->gl_context);
        SDL_DestroyWindow(graphics->window);
        free(graphics);
    }
}

int graphics_render(graphics_t graphics)
{

    static float angle = 0.0f;

    static GLfloat v0[] = {-1.0f, -1.0f, 1.0f};
    static GLfloat v1[] = {1.0f, -1.0f, 1.0f};
    static GLfloat v2[] = {1.0f, 1.0f, 1.0f};
    static GLfloat v3[] = {-1.0f, 1.0f, 1.0f};
    static GLfloat v4[] = {-1.0f, -1.0f, -1.0f};
    static GLfloat v5[] = {1.0f, -1.0f, -1.0f};
    static GLfloat v6[] = {1.0f, 1.0f, -1.0f};
    static GLfloat v7[] = {-1.0f, 1.0f, -1.0f};
    static GLubyte red[] = {255, 0, 0, 255};
    static GLubyte green[] = {0, 255, 0, 255};
    static GLubyte blue[] = {0, 0, 255, 255};
    static GLubyte white[] = {255, 255, 255, 255};
    static GLubyte yellow[] = {0, 255, 255, 255};
    static GLubyte black[] = {0, 0, 0, 255};
    static GLubyte orange[] = {255, 255, 0, 255};
    static GLubyte purple[] = {255, 0, 255, 0};

    /* Clear the color and depth buffers. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* We don't want to modify the projection matrix. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Move down the z-axis. */
    glTranslatef(0.0, 0.0, -5.0);

    /* Rotate. */
    glRotatef(angle, 0.0, 1.0, 0.0);

    if (graphics->rotating)
    {

        if (++angle > 360.0f)
        {
            angle = 0.0f;
        }
    }

    /* Send our triangle data to the pipeline. */
    glBegin(GL_TRIANGLES);

    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(blue);
    glVertex3fv(v2);

    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(blue);
    glVertex3fv(v2);
    glColor4ubv(white);
    glVertex3fv(v3);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(black);
    glVertex3fv(v5);
    glColor4ubv(orange);
    glVertex3fv(v6);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(orange);
    glVertex3fv(v6);
    glColor4ubv(blue);
    glVertex3fv(v2);

    glColor4ubv(black);
    glVertex3fv(v5);
    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(purple);
    glVertex3fv(v7);

    glColor4ubv(black);
    glVertex3fv(v5);
    glColor4ubv(purple);
    glVertex3fv(v7);
    glColor4ubv(orange);
    glVertex3fv(v6);

    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(white);
    glVertex3fv(v3);

    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(white);
    glVertex3fv(v3);
    glColor4ubv(purple);
    glVertex3fv(v7);

    glColor4ubv(white);
    glVertex3fv(v3);
    glColor4ubv(blue);
    glVertex3fv(v2);
    glColor4ubv(orange);
    glVertex3fv(v6);

    glColor4ubv(white);
    glVertex3fv(v3);
    glColor4ubv(orange);
    glVertex3fv(v6);
    glColor4ubv(purple);
    glVertex3fv(v7);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(yellow);
    glVertex3fv(v4);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(black);
    glVertex3fv(v5);

    glEnd();

    SDL_GL_SwapWindow(graphics->window);
    return 0;
}