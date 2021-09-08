#define NOBUILD_IMPLEMENTATION
#include "./include/nobuild/nobuild.h"

#define bool int
#define true 1
#define false 0

#define CFLAGS "-Wall", "-Wextra", "-std=c99", "-pedantic"
#define CFLAGS_DEBUG CFLAGS, "-g"

void setup() {
  MKDIRS("build");
  if (PATH_EXISTS("./nobuild.old")) RENAME("nobuild.old", "build/nobuild.old");
}

void build_example(bool debug) {
  if (debug)
    CMD("cc", CFLAGS_DEBUG, "-o", "build/example", "libcfg.c",
        "example/example.c");
  else
    CMD("cc", CFLAGS, "-o", "build/example", "libcfg.c", "example/example.c");
}

int main(int argc, char **argv) {
  GO_REBUILD_URSELF(argc, argv);
  setup();

  bool is_example = false;
  bool is_debug = false;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "example") == 0) {
      is_example = true;
    }

    if (strcmp(argv[i], "debug") == 0) {
      is_debug = true;
    }
  }

  if (is_example) build_example(is_debug);

  return 0;
}
