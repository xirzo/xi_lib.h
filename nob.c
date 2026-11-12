#define NOB_IMPLEMENTATION
#include "nob.h"

#define SRC_DIR "src/"
#define INCLUDE_DIR "include/"
#define BUILD_DIR "build/"
#define LIBRARY_NAME "xarena"

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  nob_mkdir_if_not_exists(BUILD_DIR);

  Nob_Cmd cmd = {0};

  nob_cmd_append(&cmd, "cc");
  nob_cmd_append(&cmd, "-c");
  nob_cmd_append(&cmd, "-Wall");
  nob_cmd_append(&cmd, "-Wextra");
  nob_cmd_append(&cmd, "-Wdouble-promotion");
  nob_cmd_append(&cmd, "-Wpedantic");
  nob_cmd_append(&cmd, "-Wconversion");
  nob_cmd_append(&cmd, "-Wshadow");
  nob_cmd_append(&cmd, "-Wundef");
  nob_cmd_append(&cmd, "-Wformat=2");
  nob_cmd_append(&cmd, "-std=c99");

  nob_cmd_append(&cmd, "-o", BUILD_DIR LIBRARY_NAME ".o");
  nob_cmd_append(&cmd, SRC_DIR "x_arena.c");
  nob_cmd_append(&cmd, "-I" INCLUDE_DIR);

  if (!nob_cmd_run(&cmd)) {
    return 1;
  }

  nob_cmd_append(&cmd, "ar", "rcs");
  nob_cmd_append(&cmd, BUILD_DIR "lib" LIBRARY_NAME ".a");
  nob_cmd_append(&cmd, BUILD_DIR LIBRARY_NAME ".o");

  if (!nob_cmd_run(&cmd)) {
    return 1;
  }

  return 0;
}
