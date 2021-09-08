#include "../libcfg.h"

int main(void) {
  const char* path = "config.cfg";

  LibCfgRoot* cfg = libcfg_read(path, 0);
  int last_error = libcfg_get_last_error();

  libcfg_free(cfg);

  return 0;
}
