#include "../libcfg.h"

int main(void) {
  const char* read_path = "config.cfg";
  const char* write_path = "gen/config.cfg";

  LibCfgRoot* cfg = libcfg_read(read_path, 0);
  int last_error = libcfg_get_last_error();

  libcfg_write(write_path, cfg);
  last_error = libcfg_get_last_error();

  libcfg_free(cfg);

  return 0;
}
