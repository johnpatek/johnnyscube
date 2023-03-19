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

#include "common.h"

void *cube_heap_malloc(cube_heap *heap, size_t size)
{
    void *block;
    heap->blocks = realloc(heap->blocks, sizeof(void *) * (heap->block_count + 1));
    block = malloc(size);
    *(heap->blocks + heap->block_count) = block;
    heap->block_count++;
    return block;
}

void *cube_heap_calloc(cube_heap *heap, size_t count, size_t size)
{
    void *block;
    heap->blocks = realloc(heap->blocks, sizeof(void *) * (heap->block_count + 1));
    block = calloc(count, size);
    *(heap->blocks + heap->block_count) = block;
    heap->block_count++;
    return block;
}

void cube_heap_clean(cube_heap *heap)
{
    size_t block_index;
    for (block_index = 0; block_index < heap->block_count; block_index++)
    {
        free(*(heap->blocks + block_index));
    }
    free(heap->blocks);
}