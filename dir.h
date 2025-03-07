#ifndef DIR_H
#define DIR_H

bool dir_exists(const char *path);
void create_dir(const char *path);
void create_dir_if_not_exists(const char *path);

#endif
