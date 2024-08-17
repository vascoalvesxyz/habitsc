#include <time.h>
#include <stdio.h>

/* helper */
time_t get_now_date();
time_t make_date(time_t time);
char* alloccat(char* home, char* name);
void red();
void blue();
void reset();

void draw_tbar(unsigned int n, char* name);
void draw_bbar(unsigned int n);

/* work by reading the file directly, no linked lists */
void print_habitn_from_file(char* path, char* name, unsigned int n); // works well
void print_habitsn_from_playlist_file(char* path, char* playlist, unsigned int n); // works well
void add_to_playlist(char* home, char* name, char* playlist); // room for slight improvement
void remove_from_playlist(char* path, char* name, char* playlist); // room for some improvment
void mark_in_file(char* path, char* name); // works for now
int search_file(FILE* f, char* name ); // works well
