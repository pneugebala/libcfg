#include "libcfg.h"

#define _LIBCFG_EOF (LIBCFG_OK + 1)

LibCfgError libcfg_last_error = LIBCFG_OK;

void libcfg_ptr_free(void* ptr) {
  if (ptr == NULL) return;
  free(ptr);
}

int libcfg_open_file(const char* path, const char* modes, FILE** out_f_ptr) {
  if ((*out_f_ptr = fopen(path, modes)) == NULL) {
    return LIBCFG_ERROR_OPENING_FILE;
  }

  return LIBCFG_OK;
}

int libcfg_getline(FILE* f_ptr, char** buffer_key, int* buffer_key_size,
                   int* buffer_key_len, char** buffer_value,
                   int* buffer_value_size, int* buffer_value_len) {
  const int buffer_inc = 25;

  if (*buffer_key_size < 1) {
    (*buffer_key_size)++;
    *buffer_key = malloc(1);
  }

  if (*buffer_value_size < 1) {
    (*buffer_value_size)++;
    *buffer_value = malloc(1);
  }

  if (*buffer_key == NULL || *buffer_value == NULL) {
    return LIBCFG_ERROR_MALLOC;
  }

  (*buffer_key)[0] = '\0';
  (*buffer_value)[0] = '\0';

  *buffer_key_len = 0;
  *buffer_value_len = 0;

  int is_eol = 0;
  int is_eof = 0;
  int has_errors = 0;
  int is_in_quotes = 0;
  int is_after_equals = 0;

  while (!is_eol && !is_eof && !has_errors) {
    char c = getc(f_ptr);

    if (c == EOF) {
      is_eof = 1;
    } else if (c == '\n') {
      is_eol = 1;
    } else if (c == '\r') {
      continue;
    } else if (!is_in_quotes &&
               (c == ' ' || c == '\t' || c == '\v' || c == '\f')) {
      continue;
    } else if (c == '"') {
      is_in_quotes = !is_in_quotes;
    } else if (c == '=') {
      if (is_after_equals) {
        has_errors = LIBCFG_ERROR_CFG_MULTIPLE_EQUALS;
        continue;
      }

      is_after_equals = 1;
    } else {
      int* index;
      int* buffer_size;
      char** buffer;

      if (is_after_equals) {
        index = buffer_value_len;
        buffer_size = buffer_value_size;
        buffer = buffer_value;
      } else {
        index = buffer_key_len;
        buffer_size = buffer_key_size;
        buffer = buffer_key;
      }

      if (*index >= (*buffer_size) - 1) {
        *buffer_size += buffer_inc;
        int new_size = *buffer_size + 1;
        char* new_buffer = realloc(*buffer, *buffer_size + 1);

        if (new_buffer == NULL) {
          has_errors = LIBCFG_ERROR_MALLOC;
          continue;
        }

        *buffer = new_buffer;
      }

      (*buffer)[(*index)++] = c;
    }
  }

  if (has_errors) return has_errors;
  if (is_in_quotes) return LIBCFG_ERROR_CFG_NO_CLOSING_QUOTE;

  (*buffer_key)[*buffer_key_len] = '\0';
  (*buffer_value)[*buffer_value_len] = '\0';

  if (is_eof) return _LIBCFG_EOF;
  return LIBCFG_OK;
}

void libcfg_free(LibCfgRoot* cfg) {
  for (int i = 0; i < cfg->sections_size; i++) {
    for (int j = 0; j < cfg->sections[i].entries_size; j++) {
      libcfg_ptr_free(cfg->sections[i].entries[j].key);
      libcfg_ptr_free(cfg->sections[i].entries[j].value);
    }

    libcfg_ptr_free(cfg->sections[i].entries);
    libcfg_ptr_free(cfg->sections[i].name);
  }

  libcfg_ptr_free(cfg->entries);
  libcfg_ptr_free(cfg->sections);
  libcfg_ptr_free(cfg);
}

LibCfgError libcfg_get_last_error() { return libcfg_last_error; }

LibCfgRoot* libcfg_read(const char* path, const int create_file) {
  char* f_modes = "r";
  if (create_file) f_modes = "ab+";

  FILE* f_ptr;
  if ((libcfg_last_error = libcfg_open_file(path, f_modes, &f_ptr)) !=
      LIBCFG_OK) {
    return NULL;
  }

  char* buffer_key = NULL;
  int buffer_key_size = 0;
  int buffer_key_len = 0;

  char* buffer_value = NULL;
  int buffer_value_size = 0;
  int buffer_value_len = 0;

  int has_errors = 0;

  LibCfgRoot* root = malloc(sizeof(LibCfgRoot));
  if (root == NULL) return LIBCFG_ERROR_MALLOC;

  root->entries = NULL;
  root->entries_size = 0;
  root->sections = NULL;
  root->sections_size = 0;

  while (!has_errors) {
    int getline_result =
        libcfg_getline(f_ptr, &buffer_key, &buffer_key_size, &buffer_key_len,
                       &buffer_value, &buffer_value_size, &buffer_value_len);

    if (getline_result == _LIBCFG_EOF) {
      break;
    }

    if (getline_result != LIBCFG_OK) {
      has_errors = getline_result;
      continue;
    }

    if (buffer_key_len == 0) {
      continue;
    }

    if (buffer_key_len > 0 && buffer_key[0] == '[') {
      if (!buffer_key[buffer_key_len - 1] == ']') {
        has_errors = LIBCFG_ERROR_CFG_NO_CLOSING_SECTION_BRACE;
        continue;
      }

      int section_name_size = buffer_key_len - 2;
      char* section_name = "";

      if (section_name_size > 0) {
        section_name = malloc(section_name_size + 1);

        if (section_name == NULL) {
          has_errors = LIBCFG_ERROR_MALLOC;
          continue;
        }

        strncpy(section_name, buffer_key + 1, section_name_size);
        section_name[section_name_size] = '\0';
      }

      LibCfgSection* new_sections = realloc(
          root->sections, (root->sections_size + 1) * sizeof(LibCfgSection));

      if (new_sections == NULL) {
        has_errors = LIBCFG_ERROR_MALLOC;
        continue;
      }

      root->sections = new_sections;
      root->sections_size++;

      LibCfgSection* section = &root->sections[root->sections_size - 1];

      section->name = section_name;
      section->entries = NULL;
      section->entries_size = 0;

      continue;
    }

    LibCfgEntry** target_entries;
    int* target_entries_size;

    if (root->sections_size == 0) {
      target_entries = &root->entries;
      target_entries_size = &root->entries_size;
    } else {
      target_entries = &root->sections[root->sections_size - 1].entries;
      target_entries_size =
          &root->sections[root->sections_size - 1].entries_size;
    }

    LibCfgEntry* new_target_entries = realloc(
        *target_entries, (*target_entries_size + 1) * sizeof(LibCfgEntry));

    if (new_target_entries == NULL) {
      has_errors = LIBCFG_ERROR_MALLOC;
      continue;
    }

    *target_entries = new_target_entries;
    (*target_entries_size)++;

    LibCfgEntry* entry = &(*target_entries)[*target_entries_size - 1];

    entry->key = malloc(buffer_key_len + 1);
    entry->value = malloc(buffer_key_len + 1);

    if (entry->key == NULL || entry->value == NULL) {
      has_errors = LIBCFG_ERROR_MALLOC;
      continue;
    }

    strcpy(entry->key, buffer_key);
    strcpy(entry->value, buffer_value);
  }

  fclose(f_ptr);

  libcfg_ptr_free(buffer_key);
  libcfg_ptr_free(buffer_value);

  if (has_errors) {
    libcfg_last_error = has_errors;
    libcfg_free(root);
    return NULL;
  }

  return root;
}

int libcfg_write(const char* path, LibCfgRoot* cfg) {}
