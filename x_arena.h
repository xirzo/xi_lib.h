#ifndef X_ARENA_H
#define X_ARENA_H

// https://zylinski.se/posts/dynamic-arrays-and-arenas/
// https://www.youtube.com/watch?v=TZ5a3gCCZYo&t=3413s
// https://nullprogram.com/blog/2023/09/27/
// https://www.youtube.com/watch?v=jgiMagdjA1s&t=355s

// TODO: scratch arena

#include <stddef.h>

typedef struct {
  void  *data;
  size_t stack_pointer;
  size_t capacity;
} X_Arena;

typedef struct {
  size_t   mark;
  X_Arena *arena;
} X_Scratch_Arena;

X_Arena x_arena_allocate(size_t size);

void x_arena_release(X_Arena *arena);

void *x_arena_push(X_Arena *arena, size_t size);

void x_arena_pop(X_Arena *arena, size_t size);

void x_arena_clear(X_Arena *arena);

X_Scratch_Arena x_arena_scratch_create(X_Arena *arena);

void x_arena_scratch_release(X_Scratch_Arena scratch);

#endif  // X_ARENA_H

#ifdef X_ARENA_IMPLEMENTATION
#ifndef X_ARENA_IMPLEMENTATION_ONCE
#define X_ARENA_IMPLEMENTATION_ONCE

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define X_ARENA_DEFAULT_ALIGN 8

static uintptr_t x_arena_align_up(uintptr_t value, uintptr_t alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

X_Arena x_arena_allocate(size_t size) {
  X_Arena arena = {0};
  if (size == 0) {
    return arena;
  }

  size_t rounded = x_arena_align_up(size, X_ARENA_DEFAULT_ALIGN);
  arena.data     = malloc(rounded);
  if (arena.data == NULL) {
    exit(EXIT_FAILURE);
  }

  arena.stack_pointer = 0;
  arena.capacity      = rounded;
  return arena;
}

void x_arena_release(X_Arena *arena) {
  if (arena == NULL || arena->data == NULL) {
    return;
  }

  free(arena->data);
  arena->data          = NULL;
  arena->stack_pointer = 0;
  arena->capacity      = 0;
}

void *x_arena_push(X_Arena *arena, size_t size) {
  if (arena == NULL || arena->data == NULL) {
    return NULL;
  }

  size_t aligned_stack_pointer =
      x_arena_align_up(arena->stack_pointer, X_ARENA_DEFAULT_ALIGN);
  if (aligned_stack_pointer > arena->capacity ||
      size > arena->capacity - aligned_stack_pointer) {
    return NULL;
  }

  void *ptr = (char *)arena->data + aligned_stack_pointer;
  memset(ptr, 0, size);
  arena->stack_pointer = aligned_stack_pointer + size;
  return ptr;
}

void x_arena_pop(X_Arena *arena, size_t size) {
  if (arena == NULL || arena->stack_pointer < size) {
    return;
  }

  void *ptr = (char *)arena->data + (arena->stack_pointer - size);
  memset(ptr, 0, size);
  arena->stack_pointer -= size;
}

void x_arena_clear(X_Arena *arena) {
  if (arena == NULL) {
    return;
  }

  arena->stack_pointer = 0;
}

X_Scratch_Arena x_arena_scratch_create(X_Arena *arena) {
  if (arena == NULL) {
    return (X_Scratch_Arena){0};
  }

  X_Scratch_Arena scratch = {0};
  scratch.mark            = arena->stack_pointer;
  scratch.arena           = arena;
  return scratch;
}

void x_arena_scratch_release(X_Scratch_Arena scratch) {
  if (scratch.arena == NULL) {
    return;
  }

  if (scratch.mark > scratch.arena->stack_pointer) {
    return;
  }

  scratch.arena->stack_pointer = scratch.mark;
}

#endif  // X_ARENA_IMPLEMENTATION_ONCE
#endif  // X_ARENA_IMPLEMENTATION
