#include "application.h"

static void audio_callback(
    void *userdata,
    Uint8 *stream,
    int len);

void audio_callback(
    void *userdata,
    Uint8 *stream,
    int len)
{
    audio_t audio;
    Uint32 current_wav_remaining;
    Uint32 bytes_needed;

    audio = (audio_t)userdata;
    bytes_needed = (Uint32)len;

    SDL_LockMutex(audio->current_wav_mutex);

    if (audio->current_wav_buffer != NULL)
    {
        current_wav_remaining = audio->current_wav_size - audio->current_wav_offset;
        if (current_wav_remaining > bytes_needed)
        {
            SDL_memcpy(
                stream,
                audio->current_wav_buffer + audio->current_wav_offset,
                bytes_needed);
            audio->current_wav_offset += bytes_needed;
            bytes_needed = 0;
        }
        else
        {
            SDL_memcpy(
                stream,
                audio->current_wav_buffer + audio->current_wav_offset,
                current_wav_remaining);
            bytes_needed -= current_wav_remaining;
            audio->current_wav_buffer = NULL;
        }
    }

    SDL_UnlockMutex(audio->current_wav_mutex);

    if (bytes_needed > 0)
    {
        SDL_memcpy(
            stream,
            audio->spec.silence,
            bytes_needed);
    }
}

int audio_initialize(audio_t audio, const char *const registry_directory)
{
    const char absolute_path[256];
    const char line_buffer[256];

    FILE *registry_file;

    sprintf(
        absolute_path,
        "%s%s%s",
        registry_directory,
        PATH_SEPARATOR,
        ".registry.txt");

    printf("loading registry: %s\n", absolute_path);
    registry_file = fopen(absolute_path, "r");

    audio->wav_buffers = NULL;
    audio->wav_sizes = NULL;
    audio->wav_count = 0;

    while (fgets(line_buffer, 255, registry_file) != NULL)
    {
        assert((1 + SDL_strlen(registry_directory) + SDL_strlen(line_buffer)) < 256);

        sprintf(
            absolute_path,
            "%s%s%s",
            registry_directory,
            PATH_SEPARATOR,
            line_buffer);

        printf("loading WAV: %s\n", absolute_path);

        audio->wav_buffers = realloc(audio->wav_buffers, sizeof(Uint8 *) * (audio->wav_count + 1));
        
        audio->wav_sizes = realloc(audio->wav_sizes, sizeof(Uint32) * (audio->wav_count + 1));
        
        SDL_LoadWAV(
            absolute_path,
            &audio->spec,
            (audio->wav_buffers + audio->wav_count),
            (audio->wav_sizes + audio->wav_count));

        audio->wav_count++;
    }

    return 0;
}

int audio_play_random(audio_t audio)
{
    int random_index;

    SDL_LockMutex(audio->current_wav_mutex);
    if (audio->current_wav_buffer == NULL)
    {
        random_index = rand() % audio->wav_count;
        audio->current_wav_buffer = *(audio->wav_buffers + random_index);
        audio->current_wav_size = *(audio->wav_sizes + random_index);
        audio->current_wav_offset = 0;
    }
    SDL_UnlockMutex(audio->current_wav_mutex);

    return 0;
}