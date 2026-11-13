#ifndef XI_ARENA_H
#define XI_ARENA_H

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
} Xi_Arena;

typedef struct {
  size_t   mark;
  Xi_Arena *arena;
} Xi_Scratch_Arena;

Xi_Arena xi_arena_allocate(size_t size);

void xi_arena_release(Xi_Arena *arena);

void *xi_arena_push(Xi_Arena *arena, size_t size);

void xi_arena_pop(Xi_Arena *arena, size_t size);

void xi_arena_clear(Xi_Arena *arena);

Xi_Scratch_Arena xi_arena_scratch_create(Xi_Arena *arena);

void xi_arena_scratch_release(Xi_Scratch_Arena scratch);

#endif  // XI_ARENA_H

#ifdef XI_ARENA_IMPLEMENTATION
#ifndef XI_ARENA_IMPLEMENTATION_ONCE
#define XI_ARENA_IMPLEMENTATION_ONCE

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define XI_ARENA_DEFAULT_ALIGN 8

static uintptr_t xi_arena_align_up(uintptr_t value, uintptr_t alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

Xi_Arena xi_arena_allocate(size_t size) {
  Xi_Arena arena = {0};
  if (size == 0) {
    return arena;
  }

  size_t rounded = xi_arena_align_up(size, XI_ARENA_DEFAULT_ALIGN);
  arena.data     = malloc(rounded);
  if (arena.data == NULL) {
    exit(EXIT_FAILURE);
  }

  arena.stack_pointer = 0;
  arena.capacity      = rounded;
  return arena;
}

void xi_arena_release(Xi_Arena *arena) {
  if (arena == NULL || arena->data == NULL) {
    return;
  }

  free(arena->data);
  arena->data          = NULL;
  arena->stack_pointer = 0;
  arena->capacity      = 0;
}

void *xi_arena_push(Xi_Arena *arena, size_t size) {
  if (arena == NULL || arena->data == NULL) {
    return NULL;
  }

  size_t aligned_stack_pointer =
      xi_arena_align_up(arena->stack_pointer, XI_ARENA_DEFAULT_ALIGN);
  if (aligned_stack_pointer > arena->capacity ||
      size > arena->capacity - aligned_stack_pointer) {
    return NULL;
  }

  void *ptr = (char *)arena->data + aligned_stack_pointer;
  memset(ptr, 0, size);
  arena->stack_pointer = aligned_stack_pointer + size;
  return ptr;
}

void xi_arena_pop(Xi_Arena *arena, size_t size) {
  if (arena == NULL || arena->stack_pointer < size) {
    return;
  }

  void *ptr = (char *)arena->data + (arena->stack_pointer - size);
  memset(ptr, 0, size);
  arena->stack_pointer -= size;
}

void xi_arena_clear(Xi_Arena *arena) {
  if (arena == NULL) {
    return;
  }

  arena->stack_pointer = 0;
}

Xi_Scratch_Arena xi_arena_scratch_create(Xi_Arena *arena) {
  if (arena == NULL) {
    return (Xi_Scratch_Arena){0};
  }

  Xi_Scratch_Arena scratch = {0};
  scratch.mark            = arena->stack_pointer;
  scratch.arena           = arena;
  return scratch;
}

void xi_arena_scratch_release(Xi_Scratch_Arena scratch) {
  if (scratch.arena == NULL) {
    return;
  }

  if (scratch.mark > scratch.arena->stack_pointer) {
    return;
  }

  scratch.arena->stack_pointer = scratch.mark;
}

#endif  // XI_ARENA_IMPLEMENTATION_ONCE
#endif  // XI_ARENA_IMPLEMENTATION
