#ifndef X_DYNAMIC_ARRAY_H_
#define X_DYNAMIC_ARRAY_H_

#include <stddef.h>

#include "xi_arena.h"

#define XI_DA_INIT(array_ptr, arena, capacity, type)                           \
  xi_da_init((array_ptr), (arena), (capacity), sizeof(type))

#define XI_DA_PUSH(array_ptr, value)                                           \
  do {                                                                         \
    __typeof__(value) _tmp = (value);                                          \
    xi_da_push((array_ptr), &_tmp);                                            \
  } while (0)

#define XI_DA_GET(array_ptr, type, index)                                      \
  (*(type *)xi_da_get((array_ptr), (index)))

#define XI_DA_POP(array_ptr, out_val_ptr)                                      \
  xi_da_pop((array_ptr), (void *)(out_val_ptr))

#define XI_DA_FOREACH(array_ptr, type, iterator_name)                          \
  for (size_t _i = 0, _keep = 1; _i < (array_ptr)->size; _i++, _keep = 1)      \
    for (type *iterator_name = (type *)xi_da_get((array_ptr), _i); _keep;      \
         _keep = 0)

#define XI_DA_INSERT(array_ptr, index, value)                                  \
  do {                                                                         \
    __typeof__(value) _tmp = (value);                                          \
    xi_da_insert((array_ptr), (index), &_tmp);                                 \
  } while (0)

#define XI_DA_REPLACE(array_ptr, index, value)                                 \
  do {                                                                         \
    __typeof__(value) _tmp = (value);                                          \
    xi_da_replace((array_ptr), (index), &_tmp);                                \
  } while (0)

#define XI_DA_REMOVE(array_ptr, index) xi_da_remove((array_ptr), (index))

#define XI_DA_CLEAR(array_ptr) xi_da_clear((array_ptr))

#define XI_DA_SHRINK(array_ptr) xi_da_shrink_to_fit((array_ptr))

#define XI_DA_SWAP(array_ptr, index1, index2)                                  \
  xi_da_swap((array_ptr), (index1), (index2))

#define XI_DA_SORT(array_ptr, compare_func)                                    \
  xi_da_sort((array_ptr), (compare_func))

enum Xi_DA_Result {
  XI_DA_SUCCESS = 0,
  XI_DA_FAILED_TO_ALLOCATE_MEMORY,
  XI_DA_EMPTY,
  XI_DA_OUT_OF_BOUNDS,
};

// The backing storage lives in an Xi_Arena, so there is nothing to free:
// xi_da_free only detaches the array from its memory.
typedef struct Xi_Dynamic_Array Xi_Dynamic_Array;

struct Xi_Dynamic_Array {
  void *data;
  Xi_Arena *arena;
  size_t element_size;
  size_t size;
  size_t capacity;
};

enum Xi_DA_Result xi_da_init(struct Xi_Dynamic_Array *array, Xi_Arena *arena,
                             size_t capacity, size_t element_size);

enum Xi_DA_Result xi_da_push(struct Xi_Dynamic_Array *array,
                             const void *element);

void *xi_da_get(struct Xi_Dynamic_Array *array, size_t index);

enum Xi_DA_Result xi_da_pop(struct Xi_Dynamic_Array *array, void *out_element);

enum Xi_DA_Result xi_da_free(struct Xi_Dynamic_Array *array);

enum Xi_DA_Result xi_da_insert(struct Xi_Dynamic_Array *array, size_t index,
                               const void *element);

enum Xi_DA_Result xi_da_replace(struct Xi_Dynamic_Array *array, size_t index,
                                const void *element);

enum Xi_DA_Result xi_da_remove(struct Xi_Dynamic_Array *array, size_t index);

enum Xi_DA_Result xi_da_clear(struct Xi_Dynamic_Array *array);

enum Xi_DA_Result xi_da_shrink_to_fit(struct Xi_Dynamic_Array *array);

enum Xi_DA_Result xi_da_swap(struct Xi_Dynamic_Array *array, size_t index1,
                             size_t index2);

enum Xi_DA_Result xi_da_sort(struct Xi_Dynamic_Array *array,
                             int (*compare)(const void *, const void *));

#ifdef X_DYNAMIC_ARRAY_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

#define XI_DA_RESIZE_MULTIPLIER 2

enum Xi_DA_Result xi_da_init(struct Xi_Dynamic_Array *array, Xi_Arena *arena,
                             size_t capacity, size_t element_size) {
  if (!array || !arena || element_size == 0) {
    return XI_DA_FAILED_TO_ALLOCATE_MEMORY;
  }

  array->arena        = arena;
  array->element_size = element_size;
  array->size         = 0;
  array->capacity     = capacity;
  array->data         = xi_arena_push(arena, capacity * element_size);

  if (!array->data) {
    return XI_DA_FAILED_TO_ALLOCATE_MEMORY;
  }

  return XI_DA_SUCCESS;
}

static enum Xi_DA_Result xi_da_resize(struct Xi_Dynamic_Array *array) {
  size_t new_capacity = array->capacity * XI_DA_RESIZE_MULTIPLIER;

  if (new_capacity == 0) {
    new_capacity = 1;
  }

  void *new_data = xi_arena_push(array->arena, new_capacity * array->element_size);
  if (!new_data) {
    return XI_DA_FAILED_TO_ALLOCATE_MEMORY;
  }

  if (array->data && array->size > 0) {
    memcpy(new_data, array->data, array->size * array->element_size);
  }

  array->data     = new_data;
  array->capacity = new_capacity;
  return XI_DA_SUCCESS;
}

enum Xi_DA_Result xi_da_push(struct Xi_Dynamic_Array *array,
                             const void *element) {
  if (!array || !element) {
    return XI_DA_FAILED_TO_ALLOCATE_MEMORY;
  }

  if (array->size >= array->capacity) {
    enum Xi_DA_Result resize_result = xi_da_resize(array);

    if (resize_result != XI_DA_SUCCESS) {
      return resize_result;
    }
  }

  char *target_address =
      (char *)array->data + array->size * array->element_size;
  memcpy(target_address, element, array->element_size);

  array->size++;
  return XI_DA_SUCCESS;
}

void *xi_da_get(struct Xi_Dynamic_Array *array, size_t index) {
  if (!array || index >= array->size) {
    return NULL;
  }

  return (char *)array->data + (array->element_size * index);
}

enum Xi_DA_Result xi_da_pop(struct Xi_Dynamic_Array *array, void *out_element) {
  if (!array || array->size == 0) {
    return XI_DA_EMPTY;
  }

  array->size--;

  if (out_element != NULL) {
    char *source_address =
        (char *)array->data + (array->size * array->element_size);
    memcpy(out_element, source_address, array->element_size);
  }

  return XI_DA_SUCCESS;
}

enum Xi_DA_Result xi_da_free(struct Xi_Dynamic_Array *array) {
  if (!array) {
    return XI_DA_SUCCESS;
  }

  // The arena owns the storage; nothing to free.
  array->data         = NULL;
  array->arena        = NULL;
  array->element_size = 0;
  array->size         = 0;
  array->capacity     = 0;

  return XI_DA_SUCCESS;
}

enum Xi_DA_Result xi_da_insert(struct Xi_Dynamic_Array *array, size_t index,
                               const void *element) {
  if (!array || index > array->size) {
    return XI_DA_OUT_OF_BOUNDS;
  }

  if (array->size >= array->capacity) {
    enum Xi_DA_Result resize_result = xi_da_resize(array);
    if (resize_result != XI_DA_SUCCESS) {
      return resize_result;
    }
  }

  char *base   = (char *)array->data;
  char *target = base + (index * array->element_size);

  if (index < array->size) {
    size_t bytes_to_move = (array->size - index) * array->element_size;
    memmove(target + array->element_size, target, bytes_to_move);
  }

  memcpy(target, element, array->element_size);
  array->size++;

  return XI_DA_SUCCESS;
}

enum Xi_DA_Result xi_da_replace(struct Xi_Dynamic_Array *array, size_t index,
                                const void *element) {
  if (!array || index >= array->size) {
    return XI_DA_EMPTY;
  }

  char *target = (char *)array->data + (index * array->element_size);
  memcpy(target, element, array->element_size);

  return XI_DA_SUCCESS;
}

enum Xi_DA_Result xi_da_remove(struct Xi_Dynamic_Array *array, size_t index) {
  if (!array || index >= array->size) {
    return XI_DA_OUT_OF_BOUNDS;
  }

  char *base   = (char *)array->data;
  char *target = base + (index * array->element_size);

  if (index < array->size - 1) {
    size_t bytes_to_move = (array->size - index - 1) * array->element_size;
    memmove(target, target + array->element_size, bytes_to_move);
  }

  array->size--;
  return XI_DA_SUCCESS;
}

enum Xi_DA_Result xi_da_clear(struct Xi_Dynamic_Array *array) {
  if (!array) {
    return XI_DA_EMPTY;
  }

  array->size = 0;
  return XI_DA_SUCCESS;
}

enum Xi_DA_Result xi_da_shrink_to_fit(struct Xi_Dynamic_Array *array) {
  if (!array) {
    return XI_DA_EMPTY;
  }

  if (array->size == array->capacity) {
    return XI_DA_SUCCESS;
  }

  if (array->size == 0) {
    array->data     = NULL;
    array->capacity = 0;
    return XI_DA_SUCCESS;
  }

  void *new_data = xi_arena_push(array->arena, array->size * array->element_size);
  if (!new_data) {
    return XI_DA_FAILED_TO_ALLOCATE_MEMORY;
  }

  memcpy(new_data, array->data, array->size * array->element_size);
  array->data     = new_data;
  array->capacity = array->size;

  return XI_DA_SUCCESS;
}

enum Xi_DA_Result xi_da_swap(struct Xi_Dynamic_Array *array, size_t index1,
                             size_t index2) {
  if (!array || index1 >= array->size || index2 >= array->size) {
    return XI_DA_OUT_OF_BOUNDS;
  }

  if (index1 == index2) {
    return XI_DA_SUCCESS;
  }

  char *ptr1 = (char *)array->data + (index1 * array->element_size);
  char *ptr2 = (char *)array->data + (index2 * array->element_size);

  char tmp[array->element_size];
  memcpy(tmp, ptr1, array->element_size);
  memcpy(ptr1, ptr2, array->element_size);
  memcpy(ptr2, tmp, array->element_size);

  return XI_DA_SUCCESS;
}

enum Xi_DA_Result xi_da_sort(struct Xi_Dynamic_Array *array,
                             int (*compare)(const void *, const void *)) {
  if (!array) {
    return XI_DA_EMPTY;
  }
  if (!compare) {
    return XI_DA_SUCCESS;
  }

  if (array->size > 1) {
    qsort(array->data, array->size, array->element_size, compare);
  }

  return XI_DA_SUCCESS;
}

#endif  // X_DYNAMIC_ARRAY_IMPLEMENTATION

#endif  // X_DYNAMIC_ARRAY_H_
