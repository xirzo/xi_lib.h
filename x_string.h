#ifndef XI_STRING_H_
#define XI_STRING_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  char *data;
  size_t length;
  size_t capacity;
} Xi_String;

Xi_String xi_string_create(const char *cstr);

Xi_String xi_string_create_empty(size_t capacity);

void xi_string_free(Xi_String *string);

const char *xi_string_get_cstr(const Xi_String *string);

Xi_String xi_string_duplicate(const Xi_String *string);

bool xi_string_resize(Xi_String *string, size_t new_capacity);

bool xi_string_append_cstr(Xi_String *string, const char *cstr);

bool xi_string_append(Xi_String *string, const Xi_String *other);

bool xi_string_append_char(Xi_String *string, char c);

bool xi_string_insert(Xi_String *string, size_t index, const char *insert_cstr);

bool xi_string_remove(Xi_String *string, size_t index, size_t count);

void xi_string_clear(Xi_String *string);

bool xi_string_printf(Xi_String *string, const char *format, ...);

bool xi_string_shrink(Xi_String *string);

bool xi_string_is_empty(const Xi_String *string);

char xi_string_get_char(const Xi_String *string, size_t index);

bool xi_string_equals(const Xi_String *s1, const Xi_String *s2);

bool xi_string_starts_with(const Xi_String *string, const char *prefix);

bool xi_string_ends_with(const Xi_String *string, const char *suffix);

void xi_string_trim(Xi_String *string);

void xi_string_to_lower(Xi_String *string);

void xi_string_to_upper(Xi_String *string);

int xi_string_find(const Xi_String *string, const char *search);

int xi_string_find_last(const Xi_String *string, const char *search);

Xi_String xi_string_substring(const Xi_String *string, size_t index, size_t length);

Xi_String *xi_string_split(const Xi_String *string, const char *delimiter,
                         size_t *out_count);

#ifdef XI_STRING_IMPLEMENTATION

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XI_STRING_CAPACITY_MULTIPLIER 2

static void *xi_string_xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (!ptr) {
    fprintf(stderr, "Fatal error: Out of memory.\n");
    abort();
  }
  return ptr;
}

Xi_String xi_string_create(const char *cstr) {
  assert(cstr != NULL);
  size_t length = strlen(cstr);
  size_t capacity = length * XI_STRING_CAPACITY_MULTIPLIER;

  char *data = xi_string_xmalloc(sizeof(char) * (capacity + 1));
  strcpy(data, cstr);

  return (Xi_String){
      .data = data,
      .length = length,
      .capacity = capacity,
  };
}

Xi_String xi_string_create_empty(size_t capacity) {
  char *data = xi_string_xmalloc(capacity + 1);
  data[0] = '\0';

  return (Xi_String){
      .data = data,
      .length = 0,
      .capacity = capacity,
  };
}

void xi_string_free(Xi_String *string) {
  if (!string) {
    return;
  }

  if (string->data) {
    free(string->data);
  }

  string->data = NULL;
  string->length = 0;
  string->capacity = 0;
}

const char *xi_string_get_cstr(const Xi_String *string) {
  assert(string != NULL);
  return string->data;
}

Xi_String xi_string_duplicate(const Xi_String *string) {
  assert(string != NULL);
  assert(string->data != NULL);

  char *new_data = xi_string_xmalloc(string->capacity + 1);
  memcpy(new_data, string->data, string->length + 1);

  return (Xi_String){
      .data = new_data,
      .length = string->length,
      .capacity = string->capacity,
  };
}

bool xi_string_resize(Xi_String *string, size_t new_capacity) {
  assert(string != NULL);
  assert(string->capacity >= string->length);

  if (new_capacity == string->capacity) {
    return true;
  }

  char *new_data = realloc(string->data, new_capacity + 1);

  if (!new_data) {
    return false;
  }

  string->data = new_data;
  string->capacity = new_capacity;

  return true;
}

bool xi_string_append_cstr(Xi_String *string, const char *cstr) {
  assert(string != NULL);
  assert(string->capacity >= string->length);

  size_t cstr_length = strlen(cstr);
  size_t new_total_length = string->length + cstr_length;

  if (new_total_length > string->capacity &&
      !xi_string_resize(string, new_total_length)) {
    return false;
  }

  memcpy(string->data + string->length, cstr, cstr_length + 1);
  string->length = new_total_length;

  return true;
}

bool xi_string_append(Xi_String *string, const Xi_String *other) {
  assert(string != NULL);
  assert(other != NULL);
  assert(string->capacity >= string->length);

  size_t new_total_length = string->length + other->length;

  if (new_total_length > string->capacity) {
    if (!xi_string_resize(string, new_total_length)) {
      return false;
    }
  }

  memcpy(string->data + string->length, other->data, other->length + 1);
  string->length = new_total_length;

  return true;
}

bool xi_string_append_char(Xi_String *string, char c) {
  assert(string != NULL);
  assert(string->capacity >= string->length);

  size_t new_total_length = string->length + 1;

  if (new_total_length > string->capacity) {
    if (!xi_string_resize(string, new_total_length)) {
      return false;
    }
  }

  *(string->data + string->length) = c;
  string->data[new_total_length] = '\0';
  string->length = new_total_length;

  return true;
}

bool xi_string_insert(Xi_String *string, size_t index, const char *insert_cstr) {
  assert(string != NULL);
  assert(string->capacity >= string->length);
  assert(index <= string->length);

  size_t cstr_length = strlen(insert_cstr);
  if (cstr_length == 0) {
    return true;
  }

  size_t new_total_length = string->length + cstr_length;

  if (new_total_length > string->capacity) {
    if (!xi_string_resize(string, new_total_length)) {
      return false;
    }
  }

  size_t bytes_to_move = string->length - index + 1;
  memmove(string->data + index + cstr_length, string->data + index,
          bytes_to_move);
  memcpy(string->data + index, insert_cstr, cstr_length);

  string->length = new_total_length;
  return true;
}

bool xi_string_remove(Xi_String *string, size_t index, size_t count) {
  assert(string != NULL);
  assert(string->capacity >= string->length);
  assert(index <= string->length);

  if (count == 0) {
    return true;
  }

  if (index + count > string->length) {
    count = string->length - index;
  }

  size_t tail_length = string->length - index - count;
  size_t new_total_length = string->length - count;
  memmove(string->data + index, string->data + index + count, tail_length);

  string->length = new_total_length;
  string->data[string->length] = '\0';

  return true;
}

void xi_string_clear(Xi_String *string) {
  assert(string != NULL);

  for (size_t i = 0; i < string->length; i++) {
    string->data[i] = '\0';
  }

  string->length = 0;
}

bool xi_string_printf(Xi_String *string, const char *format, ...) {
  assert(string != NULL);
  assert(format != NULL);

  va_list args;
  va_start(args, format);
  int needed = vsnprintf(NULL, 0, format, args);
  va_end(args);

  if (needed < 0) {
    return false;
  }

  size_t needed_size = (size_t)needed;

  if (needed_size > string->capacity &&
      !xi_string_resize(string, needed_size)) {
    return false;
  }

  va_start(args, format);
  vsnprintf(string->data, needed_size + 1, format, args);
  va_end(args);

  string->length = needed_size;
  return true;
}

bool xi_string_shrink(Xi_String *string) {
  assert(string != NULL);

  if (string->length == string->capacity) {
    return true;
  }

  char *tmp = realloc(string->data, string->length + 1);
  if (!tmp) {
    return false;
  }

  string->data = tmp;
  string->capacity = string->length;
  return true;
}

bool xi_string_is_empty(const Xi_String *string) {
  assert(string != NULL);
  return string->length == 0;
}

char xi_string_get_char(const Xi_String *string, size_t index) {
  assert(string != NULL);
  assert(index < string->length);

  return string->data[index];
}

bool xi_string_equals(const Xi_String *s1, const Xi_String *s2) {
  assert(s1 != NULL);
  assert(s2 != NULL);
  if (s1->length != s2->length) {
    return false;
  }

  return memcmp(s1->data, s2->data, s1->length) == 0;
}

bool xi_string_starts_with(const Xi_String *string, const char *prefix) {
  assert(string != NULL);
  assert(prefix != NULL);

  size_t prefix_length = strlen(prefix);

  if (prefix_length > string->length) {
    return false;
  }

  return memcmp(string->data, prefix, prefix_length) == 0;
}

bool xi_string_ends_with(const Xi_String *string, const char *suffix) {
  assert(string != NULL);
  assert(suffix != NULL);

  size_t suffix_length = strlen(suffix);

  if (suffix_length > string->length) {
    return false;
  }

  size_t start_index = string->length - suffix_length;

  return memcmp(string->data + start_index, suffix, suffix_length) == 0;
}

void xi_string_trim(Xi_String *string) {
  assert(string != NULL);

  if (string->length == 0) {
    return;
  }

  size_t start = 0;
  while (start < string->length &&
         isspace((unsigned char)string->data[start])) {
    start++;
  }

  if (start == string->length) {
    string->data[0] = '\0';
    string->length = 0;
    return;
  }

  size_t end = string->length - 1;
  while (end > start && isspace((unsigned char)string->data[end])) {
    end--;
  }

  size_t new_length = (end - start) + 1;

  if (start > 0) {
    memmove(string->data, string->data + start, new_length);
  }

  string->data[new_length] = '\0';
  string->length = new_length;
}

void xi_string_to_lower(Xi_String *string) {
  assert(string != NULL);

  if (string->length == 0) {
    return;
  }

  for (size_t i = 0; i < string->length; i++) {
    string->data[i] = (char)tolower((unsigned char)string->data[i]);
  }
}

void xi_string_to_upper(Xi_String *string) {
  assert(string != NULL);

  if (string->length == 0) {
    return;
  }

  for (size_t i = 0; i < string->length; i++) {
    string->data[i] = (char)toupper((unsigned char)string->data[i]);
  }
}

int xi_string_find(const Xi_String *string, const char *search) {
  assert(string != NULL);
  assert(search != NULL);

  size_t search_length = strlen(search);

  if (search_length == 0 || search_length > string->length) {
    return -1;
  }

  size_t limit = string->length - search_length;
  for (size_t i = 0; i <= limit; i++) {
    if (memcmp(string->data + i, search, search_length) == 0) {
      return (int)i;
    }
  }

  return -1;
}

int xi_string_find_last(const Xi_String *string, const char *search) {
  assert(string != NULL);
  assert(search != NULL);

  size_t search_length = strlen(search);

  if (search_length == 0 || search_length > string->length) {
    return -1;
  }

  size_t i = string->length - search_length;

  while (true) {
    if (memcmp(string->data + i, search, search_length) == 0) {
      return (int)i;
    }

    if (i == 0) {
      break;
    }

    i--;
  }

  return -1;
}

Xi_String xi_string_substring(const Xi_String *string, size_t index, size_t length) {
  assert(string != NULL);
  assert(string->capacity >= string->length);
  assert(index <= string->length);

  if (index + length > string->length) {
    length = string->length - index;
  }

  Xi_String substring = xi_string_create_empty(length);

  if (length > 0) {
    memcpy(substring.data, string->data + index, length);
  }

  substring.length = length;
  substring.data[length] = '\0';

  return substring;
}

Xi_String *xi_string_split(const Xi_String *string, const char *delimiter,
                         size_t *out_count) {
  assert(string != NULL);
  assert(delimiter != NULL);
  assert(out_count != NULL);

  size_t delim_length = strlen(delimiter);

  assert(delim_length > 0);

  size_t count = 1;
  size_t current_pos = 0;

  while (current_pos + delim_length <= string->length) {
    if (memcmp(string->data + current_pos, delimiter, delim_length) == 0) {
      count++;
      current_pos += delim_length;
    } else {
      current_pos++;
    }
  }

  Xi_String *results = malloc(count * sizeof(Xi_String));
  if (!results) {
    *out_count = 0;
    return NULL;
  }

  size_t start_idx = 0;
  size_t result_idx = 0;
  current_pos = 0;

  while (current_pos + delim_length <= string->length) {
    if (memcmp(string->data + current_pos, delimiter, delim_length) == 0) {
      size_t chunk_length = current_pos - start_idx;
      results[result_idx++] = xi_string_substring(string, start_idx, chunk_length);

      current_pos += delim_length;
      start_idx = current_pos;
    } else {
      current_pos++;
    }
  }

  results[result_idx] =
      xi_string_substring(string, start_idx, string->length - start_idx);

  *out_count = count;
  return results;
}

#endif  // XI_STRING_IMPLEMENTATION

#endif  // XI_STRING_H_
