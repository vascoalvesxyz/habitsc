#ifndef _HABITS_H_
#define _HABITS_H_

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define ALL "all.playlist"
#define TRUE 1
#define FALSE 0

#define ERR_NOFILE -1
#define ERR_NOTFOUND 0

typedef unsigned int uint;

/* h_files */
/* int because they return ERR_NOFILE and ERR_NOTFOUND */
char* alloccat(char* home, char* name);
int file_delete_line(char* path, char* name); 
int file_insert_line_fgets(char* path, char* where, char* name);
int line_replace_fgets(char* path, char* where, char* name);
int file_prepend(char* path, char* what);
int file_append(char* path, char* what);
int file_touch(char* path);
int file_check(char* path);
#endif
