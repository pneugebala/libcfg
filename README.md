# libcfg

Library for reading/writing simple configuration files.

Configuration files consist of key-value pairs divided up into sections.

No extra dependencies.

## Example

Configuration file:

```properties
version = 1
name = Test

[Section 1]
name = "Test 1"
state = true

[Section 2]
name = "Test 2"
state = false
has_changes = true
```

Code:

```c
#include "libcfg.h"

int main(void) {
  const char* path = "config.cfg";

  // Reading config file or creating new one if it
  // does not exist.
  LibCfgRoot* cfg = libcfg_read(path, 1);

  // Set the second entry's value to 'Hello'.
  libcfg_modify_entry(&cfg->entries[1], NULL, "Hello");

  // Add a new entry with key 'is_new' and value 'true'.
  libcfg_add_entry(cfg, "is_new", "true");

  // Remove the second section.
  libcfg_remove_section(cfg, &cfg->sections[1]);

  // Write the config to file.
  libcfg_write(path, cfg);

  // We can get the last error using this.
  // Returns 'LIBCFG_OK' when no error.
  last_error = libcfg_get_last_error();

  // Free the LibCfgRoot struct.
  libcfg_free(cfg);

  return 0;
}
```

Extended example: [example/example.c](example/example.c)
