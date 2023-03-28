/*
** Copyright 2023 John R. Patek Sr.
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

#ifndef CUBE_APPLICATION_COMMON_H
#define CUBE_APPLICATION_COMMON_H

#define CUBE_SUCCESS 0
#define CUBE_FAILURE 1

#ifdef _WIN32
#include <Windows.h>
#else
#include <errno.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <assert.h>
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

#ifndef NDEBUG
#define CUBE_DEBUG
#endif

typedef struct
{
    void **blocks;
    size_t block_count;
} cube_heap;

void cube_heap_push(cube_heap *heap, void *block);
void *cube_heap_malloc(cube_heap *heap, size_t size);
void *cube_heap_calloc(cube_heap *heap, size_t count, size_t size);
void cube_heap_clean(cube_heap *heap);

#define CUBE_PUSH(BLOCK) cube_heap_push(&heap, BLOCK)
#define CUBE_MALLOC(SIZE) cube_heap_malloc(&heap, SIZE)
#define CUBE_CALLOC(COUNT, SIZE) cube_heap_calloc(&heap, COUNT, SIZE)

#define CUBE_BEGIN_FUNCTION     \
    cube_heap heap = {          \
        .blocks = NULL,         \
        .block_count = 0,       \
    };                          \
    int cube_result;            \
    VkResult vk_result;         \
    cube_result = CUBE_SUCCESS; \
    vk_result = VK_SUCCESS;

#define CUBE_END_FUNCTION       \
    goto done;                  \
    error:                      \
    cube_result = CUBE_FAILURE; \
    done:                       \
    cube_heap_clean(&heap);     \
    return cube_result;

#define CUBE_ASSERT(COND, MESSAGE) \
    if (!(COND))                   \
    {                              \
        fprintf(                   \
            stderr,                \
            "%s:%d: %s: %s\n",     \
            __FILE__,              \
            __LINE__,              \
            __func__,              \
            MESSAGE);              \
        goto error;                \
    }

#define VK_CHECK_RESULT(FUNC)             \
    vk_result = FUNC;                     \
    if (vk_result != VK_SUCCESS)          \
    {                                     \
        fprintf(                          \
            stderr,                       \
            "%s:%d: %s: %s failed(%d)\n", \
            __FILE__,                     \
            __LINE__,                     \
            __func__,                     \
            #FUNC,                        \
            vk_result);                   \
        goto error;                       \
    }

#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) \
                                                         : (x))

#endif