#ifndef _HABITS_H_
#define _HABITS_H_

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define ALL "all.playlist"
#define TRUE 1
#define FALSE 0

/* h_draw */
void red();
void blue();
void reset();
void draw_tbar(unsigned int n, char* name);
void draw_bbar(unsigned int n);
void draw_hseperators(unsigned int n);

/* h_files */
/* -1 = bad path, 0 = not found, 1 = successful */
char* alloccat(char* home, char* name);
int file_delete_line(char* path, char* name); 
int file_insert_line_fgets(char* path, char* where, char* name);
int line_replace_fgets(char* path, char* where, char* name);
int file_prepend(char* path, char* what);
int file_append(char* path, char* what);
int file_touch(char* path);

/* h_habits */
time_t date_midnight(time_t time);
void habit_mark(char* habit_path); 
int habit_create(char* habit_path); /* returns 1 if already created, 0 if can be added to ALL */
int habit_delete(char* habit_path);
void habit_rename(char* path, char* old_name, char* new_name);
void habit_print(char* home, char* habit_name, unsigned int total_days); 

/* h_playlist */
void playlist_create(char* playlist_path);
void playlist_delete(char* playlist_path); 
void playlist_add(char* home_path, char* name, char* playlist); 
void playlist_remove(char* home_path, char* name, char* playlist); 

void playlist_print(char* home, char* playlist_name, unsigned int n); 
#endif
