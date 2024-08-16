#include <time.h>
/* helper */
time_t get_now_date();
time_t make_date(time_t time);
char* alloc_real_path(char* home, char* name);
void red();
void blue();
void reset();

/* work by reading the file directly, no linked lists */
void print_habitn_from_file(char* path, char* name, int n); // ✅✅ works
void print_habitsn_from_playlist_file(char* path, char* playlist, int n); // ✅✅ works
void add_to_playlist(char* home, char* name, char* playlist); // ✅ appends to playlist file
void remove_from_playlist(char* path, char* name, char* playlist); // removes from playlist
void mark_in_file(char* path, char* name); // 
