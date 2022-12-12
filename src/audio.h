#include "common.h"

struct audio_s
{
    SDL_AudioDeviceID device_id;
    SDL_AudioSpec spec;

    Uint8 ** wav_buffers;
    Uint32 * wav_sizes;
    Uint32 wav_count;

    SDL_mutex *current_wav_mutex;
    Uint8 * current_wav_buffer;
    Uint32 current_wav_size;
    Uint32 current_wav_offset;
};
typedef struct audio_s *audio_t;

int audio_create(audio_t* create);

int audio_initialize(audio_t audio, const char * const registry_directory);

int audio_play_random(audio_t audio);

int audio_destroy(audio_t audio);