#include "../libcfg.h"

int main(void) {
  const char* read_path = "config.cfg";
  const char* write_path = "gen/config.cfg";

  LibCfgRoot* cfg = libcfg_read(read_path, 0);
  int last_error = libcfg_get_last_error();

  cfg->entries[0].value = "2";
  cfg->entries[1].value = "Hello";

  libcfg_add_entry(cfg, "is_new", "true");
  libcfg_remove_entry(cfg, &cfg->entries[1]);

  libcfg_add_section_entry(&cfg->sections[1], "is_new", "true");
  libcfg_remove_section_entry(&cfg->sections[1], &cfg->sections[1].entries[1]);

  libcfg_add_section(cfg, "Section 3");
  libcfg_remove_section(cfg, &cfg->sections[0]);

  libcfg_add_section_entry(&cfg->sections[1], "is_new", "N/A");

  libcfg_write(write_path, cfg);
  last_error = libcfg_get_last_error();

  libcfg_free(cfg);

  return 0;
}
