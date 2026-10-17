#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct Arena {
    char *buffer;
    size_t capacity;
    size_t offset;
} Arena;

// Initialize a new memory arena with a specific capacity
Arena* arena_create(size_t capacity);

// Allocate memory from the arena
void* arena_alloc(Arena *arena, size_t size);

// Reset the arena offset (frees all allocations instantly without individual free calls)
void arena_reset(Arena *arena);

// Free the arena buffer and structure entirely
void arena_free(Arena *arena);

#endif // ARENA_H