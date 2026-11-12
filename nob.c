#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_DIR "build/"
#define TESTS_DIR "tests/"
#define LIBRARY_NAME "xarena"

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  nob_mkdir_if_not_exists(BUILD_DIR);

  Nob_Cmd cmd = {0};

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
  nob_cmd_append(&cmd, "-o" BUILD_DIR "tests");
  nob_cmd_append(&cmd, TESTS_DIR "test.c");

  if (!nob_cmd_run(&cmd)) {
    return 1;
  }

  return 0;
}
