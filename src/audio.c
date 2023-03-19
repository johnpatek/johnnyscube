#include "application.h"

static void audio_callback(
    void *userdata,
    Uint8 *stream,
    int len);

int audio_create(audio_t *audio, const char *resource_directory)
{
    int status = CUBE_SUCCESS;
    *audio = calloc(1, sizeof(struct audio_s));
    return 0;
}

int audio_play_random(audio_t audio)
{
    int random_index;
    random_index = 0;
    /*
    SDL_LockMutex(audio->current_mutex);
    if (audio->current == NULL)
    {
        random_index = rand() % audio->wav_count;
        audio->current = audio->wavs + random_index;
        audio->current_offset = 0;
    }
    SDL_UnlockMutex(audio->current_mutex);
    */
    return 0;
}

int audio_destroy(audio_t audio)
{
    int wav_index;
    wav_index = 0;
    /*
    SDL_PauseAudioDevice(audio->device_id, SDL_TRUE);

    SDL_DestroyMutex(audio->current_mutex);

    SDL_CloseAudioDevice(audio->device_id);

    for (wav_index = 0; wav_index < audio->wav_count; wav_index++)
    {
        SDL_FreeWAV((audio->wavs + wav_index)->buffer);
    }

    free(audio->wavs);
    */
    free(audio);

    return 0;
}

void audio_callback(
    void *userdata,
    Uint8 *stream,
    int len)
{
    audio_t audio;
    Uint32 current_remaining;
    Uint32 bytes_needed;

    audio = (audio_t)userdata;
    bytes_needed = (Uint32)len;

    SDL_memset(
        stream,
        audio->spec.silence,
        bytes_needed);

    SDL_LockMutex(audio->current_mutex);

    if (audio->current != NULL)
    {
        current_remaining = audio->current->size - audio->current_offset;
        if (current_remaining > bytes_needed)
        {
            SDL_memcpy(
                stream,
                audio->current->buffer + audio->current_offset,
                bytes_needed);
            audio->current_offset += bytes_needed;
        }
        else
        {
            SDL_memcpy(
                stream,
                audio->current->buffer + audio->current_offset,
                current_remaining);
            audio->current = NULL;
        }
    }
    SDL_UnlockMutex(audio->current_mutex);
}