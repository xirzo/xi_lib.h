#define NOB_IMPLEMENTATION
#include "nob.h"

#include <string.h>
#include <stdio.h>

#define BUILD_DIR "build/"
#define TESTS_DIR "tests/"

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  nob_mkdir_if_not_exists(BUILD_DIR);

  Nob_Cmd cmd = {0};

  const char *test_sources[] = {
      TESTS_DIR "test.c",
      TESTS_DIR "test_xi_string_lifecycle.c",
      TESTS_DIR "test_xi_string_mutate.c",
      TESTS_DIR "test_xi_dynamic_array.c",
  };

  size_t tests_count = sizeof(test_sources) / sizeof(test_sources[0]);

  for (size_t i = 0; i < tests_count; i++) {
    cmd.count = 0;

    const char *source = test_sources[i];
    const char *base   = source + strlen(TESTS_DIR);
    char test_name[64] = {0};
    size_t base_length = strlen(base) - strlen(".c");
    memcpy(test_name, base, base_length < sizeof(test_name) - 1 ? base_length
                                                               : sizeof(test_name) - 1);

    static char output_path[256] = {0};
    snprintf(output_path, sizeof(output_path), BUILD_DIR "%s", test_name);

    nob_cmd_append(&cmd, "cc");
    nob_cmd_append(&cmd, "-Wall");
    nob_cmd_append(&cmd, "-Werror");
    nob_cmd_append(&cmd, "-Wextra");
    nob_cmd_append(&cmd, "-Wdouble-promotion");
    nob_cmd_append(&cmd, "-Wpedantic");
    nob_cmd_append(&cmd, "-Wconversion");
    nob_cmd_append(&cmd, "-Wshadow");
    nob_cmd_append(&cmd, "-Wundef");
    nob_cmd_append(&cmd, "-Wformat=2");
    nob_cmd_append(&cmd, "-std=c99");
    nob_cmd_append(&cmd, "-g");
    nob_cmd_append(&cmd, "-O0");
    nob_cmd_append(&cmd, "-fsanitize=address,undefined");
    nob_cmd_append(&cmd, "-fno-omit-frame-pointer");
    nob_cmd_append(&cmd, "-fno-sanitize-recover=all");
    nob_cmd_append(&cmd, "-I./");
    nob_cmd_append(&cmd, "-o");
    nob_cmd_append(&cmd, output_path);
    nob_cmd_append(&cmd, test_sources[i]);

    if (!nob_cmd_run(&cmd)) {
      return 1;
    }

    cmd.count = 0;
    nob_cmd_append(&cmd, output_path);

    if (!nob_cmd_run(&cmd)) {
      return 1;
    }
  }

  return 0;
}
