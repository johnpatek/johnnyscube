#include <GL/glu.h>
#include <GL/gl.h>
#include <SDL3/SDL.h>

#include <stdio.h>

#define SDL_OK 0

#define SOUND_OFF 0
#define SOUND_ON 1

struct application_context
{
    int width;
    int height;
    SDL_Window *window;
    SDL_GLContext gl_context;
    SDL_bool loop;
    SDL_bool rotate;
    SDL_Thread *audio_thread;
    SDL_atomic_t sound_state;
};

static int get_screen_size(struct application_context *const context);

static void main_loop(struct application_context *const context);

static void opengl_initialize(struct application_context *const context);

static void handle_event(struct application_context *const context, const SDL_Event *const event);

static void handle_keyboard_event(struct application_context *const context, const SDL_KeyboardEvent *const keyboard_event);

static void audio_play(struct application_context *const context);

static void opengl_render(struct application_context *const context);

static void audio_callback(void *userdata, Uint8 *stream, int len);

int main(int agrc, char **argv)
{
    int initialized;
    int status;
    struct application_context context;

    SDL_Event event;

    initialized = 0;

    status = SDL_Init(SDL_INIT_EVERYTHING);
    if (status != SDL_OK)
    {
        goto error;
    }

    context.window = NULL;
    context.gl_context = NULL;
    context.rotate = SDL_FALSE;
    initialized = 1;

    status = get_screen_size(&context);
    if (status != SDL_OK)
    {
        goto error;
    }

    context.window = SDL_CreateWindow(
        "Johnny's Cube",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        context.width,
        context.height,
        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
    if (context.window == NULL)
    {
        goto error;
    }

    context.gl_context = SDL_GL_CreateContext(context.window);
    if (context.gl_context == NULL)
    {
        goto error;
    }

    context.loop = SDL_TRUE;

    main_loop(&context);

    goto done;
error:
    printf("main: %s\n", SDL_GetError());
done:
    if (initialized == 1)
    {
        if (context.window != NULL)
        {
            SDL_DestroyWindow(context.window);
        }

        if (context.gl_context != NULL)
        {
            SDL_GL_DeleteContext(context.gl_context);
        }

        SDL_Quit();
    }
    return 0;
}

int get_screen_size(struct application_context *const context)
{
    int status;
    SDL_DisplayMode display_mode;

    status = SDL_GetCurrentDisplayMode(0, &display_mode);
    if (status == SDL_OK)
    {
        context->width = display_mode.w;
        context->height = display_mode.h;
    }

    return status;
}


static void main_loop(struct application_context *const context)
{
    SDL_Event event;

    SDL_AtomicSet(&context->sound_state, SOUND_OFF);

    opengl_initialize(context);

    while (context->loop)
    {
        if (SDL_PollEvent(&event) > 0)
        {
            handle_event(context, &event);
        }
        opengl_render(context);
    }
}

void opengl_initialize(struct application_context *const context)
{
    float ratio = (float)context->width / (float)context->height;

    /* Our shading model--Gouraud (smooth). */
    glShadeModel(GL_SMOOTH);

    /* Culling. */
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    /* Set the clear color. */
    glClearColor(0, 0, 0, 0);

    /* Setup our viewport. */
    glViewport(0, 0, context->width, context->height);

    /*
     * Change to the projection matrix and set
     * our viewing volume.
     */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /*
     * EXERCISE:
     * Replace this with a call to glFrustum.
     */
    gluPerspective(60.0, ratio, 1.0, 1024.0);
}

void handle_event(struct application_context *const context, const SDL_Event *const event)
{
    switch (event->type)
    {
    case SDL_KEYDOWN:
        handle_keyboard_event(context, (const SDL_KeyboardEvent *const)event);
    }
}

void handle_keyboard_event(struct application_context *const context, const SDL_KeyboardEvent *const keyboard_event)
{
    SDL_Keysym key;

    key = keyboard_event->keysym;

    if (keyboard_event->repeat == 0)
    {
        switch (key.sym)
        {
        case SDLK_ESCAPE:
            context->loop = SDL_FALSE;
            break;
        default:
            context->rotate = !context->rotate;
            audio_play(context);
        }
    }
}

void audio_play(struct application_context *const context)
{
    SDL_AudioSpec wav_spec;
    Uint32 wav_length;
    Uint8 *wav_buffer;
    SDL_AudioDeviceID audio_device_id;
    int audio_device_index;

    for (audio_device_index = 0; audio_device_index < SDL_GetNumAudioDevices(0); audio_device_index++)
    {
        printf("device index: %d \n device name: %s \n\n", audio_device_index, SDL_GetAudioDeviceName(audio_device_index, 0));
    }

    if (SDL_LoadWAV("../sounds/bark.wav", &wav_spec, &wav_buffer, &wav_length) == NULL)
    {
        fprintf(stderr, "LoadWave failed\n");
    }

    wav_spec.callback = audio_callback;

    audio_device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (audio_device_id > 0)
    {
        SDL_QueueAudio(audio_device_id, wav_buffer, wav_length);
        SDL_PauseAudioDevice(audio_device_id, 0);
        SDL_PauseAudioDevice(audio_device_id, 1);
        SDL_CloseAudioDevice(audio_device_id);
    }
    else
    {
        fprintf(stderr, "failed to open audio device(%s)\n", SDL_GetError());
    }
}

void opengl_render(struct application_context *const context)
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

    if (context->rotate)
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

    /*
     * EXERCISE:
     * Draw text telling the user that 'Spc'
     * pauses the rotation and 'Esc' quits.
     * Do it using vetors and textured quads.
     */

    /*
     * Swap the buffers. This this tells the driver to
     * render the next frame from the contents of the
     * back-buffer, and to set all rendering operations
     * to occur on what was the front-buffer.
     *
     * Double buffering prevents nasty visual tearing
     * from the application drawing on areas of the
     * screen that are being updated at the same time.
     */
    SDL_GL_SwapWindow(context->window);
}

void audio_callback(void *userdata, Uint8 *stream, int len)
{
}