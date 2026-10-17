#include "arena.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Arena* arena_create(size_t capacity) {
    Arena *arena = (Arena*)malloc(sizeof(Arena));
    if (!arena) {
        fprintf(stderr, "Failed to allocate memory for Arena structure.\n");
        return NULL;
    }
    
    arena->buffer = (char*)malloc(capacity);
    if (!arena->buffer) {
        fprintf(stderr, "Failed to allocate memory for Arena buffer.\n");
        free(arena);
        return NULL;
    }
    
    arena->capacity = capacity;
    arena->offset = 0;
    return arena;
}

void* arena_alloc(Arena *arena, size_t size) {
    // Align size to 8 bytes for safe memory alignment
    size_t aligned_size = (size + 7) & ~7;
    
    if (arena->offset + aligned_size > arena->capacity) {
        fprintf(stderr, "Arena out of memory! Capacity: %zu, Requested: %zu\n", arena->capacity, aligned_size);
        return NULL;
    }
    
    void *ptr = &arena->buffer[arena->offset];
    arena->offset += aligned_size;
    return ptr;
}

void arena_reset(Arena *arena) {
    if (arena) {
        arena->offset = 0;
    }
}

void arena_free(Arena *arena) {
    if (arena) {
        if (arena->buffer) {
            free(arena->buffer);
        }
        free(arena);
    }
}