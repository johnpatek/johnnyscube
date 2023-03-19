#ifndef AUDIO_H
#define AUDIO_H

#include "common.h"

typedef struct 
{
    Uint8 *buffer;
    Uint32 size;
} wav;

struct audio_s
{
    SDL_AudioDeviceID device_id;
    SDL_AudioSpec spec;

    wav *wavs;
    uint32_t wav_count;

    SDL_mutex *current_mutex;
    wav *current;
    uint32_t current_offset;
};
typedef struct audio_s *audio_t;

int audio_create(
    audio_t* create, 
    const char * resource_directory);

int audio_play_random(audio_t audio);

int audio_destroy(audio_t audio);

#endif