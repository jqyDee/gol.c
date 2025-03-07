#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "dir.h"

bool dir_exists(const char *path) {
  struct stat info;
  return (stat(path, &info) == 0 && (info.st_mode & S_IFDIR));
}

void create_dir(const char *path) {
  if (mkdir(path, 0777) == -1) {
    fprintf(stderr, "error: %s\n", strerror(errno));
  }
}

void create_dir_if_not_exists(const char *path) {
  if (!dir_exists(path)) {
    create_dir(path);
  }
}
