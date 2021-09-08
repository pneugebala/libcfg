#ifndef _H_LIBCFG
#define _H_LIBCFG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * A config key-value pair config entry.
 * Part of the root struct or an section.
 */
typedef struct {
  char* key;
  char* value;
} LibCfgEntry;

/*
 * A section inside the config. Has a name
 * and contains all sub entries.
 */
typedef struct {
  char* name;

  LibCfgEntry* entries;
  int entries_size;
} LibCfgSection;

/*
 * The root struct for a config. Containts
 * all root entries and sections.
 */
typedef struct {
  LibCfgEntry* entries;
  int entries_size;

  LibCfgSection* sections;
  int sections_size;
} LibCfgRoot;

/*
 * Error codes for libcfg.
 */
typedef enum {
  LIBCFG_OK = 0,
  LIBCFG_ERROR_OPENING_FILE = -1,
  LIBCFG_ERROR_MALLOC = -2,
  LIBCFG_ERROR_WRITING_FILE = -3,
  LIBCFG_ERROR_CFG_NO_CLOSING_QUOTE = -10,
  LIBCFG_ERROR_CFG_MULTIPLE_EQUALS = -11,
  LIBCFG_ERROR_CFG_NO_CLOSING_SECTION_BRACE = -12,
} LibCfgError;

/*
 * Free given LibCfgRoot struct.
 */
void libcfg_free(LibCfgRoot* cfg);

/*
 * Get the error code from the last libcfg function call.
 * Returns LIBCFG_OK when last call was successfull.
 */
LibCfgError libcfg_get_last_error();

/*
 * Read the file at path and return a LibCfgRoot struct.
 * Create a new file if none at path when create_file is not 0.
 * Returns NULL and sets an error to be read with libcfg_get_last_error()
 * when unsuccessful.
 */
LibCfgRoot* libcfg_read(const char* path, const int create_file);

/*
 * Write the file at path with given LibCfgRoot struct.
 * Returns 0 when successful or a negative error code when
 * not. Error can also be read using libcfg_get_last_error().
 */
int libcfg_write(const char* path, LibCfgRoot* cfg);

#endif
