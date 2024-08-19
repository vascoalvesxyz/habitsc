#include "config.h"
#include "habits.h"

/* prints habits from a dot playlist file */
void playlist_print(char* home, char* playlist_name, unsigned int n) {

    char* playlist_path = alloccat(home, playlist_name);
    int file_to_read = open(playlist_path, O_RDONLY);

    if(file_to_read == -1) {
        printf("Playlist not found!");
    }
    else {

        /* make sure it's not over ten MB, just to be safe */
        int total_chars = lseek(file_to_read, 0, SEEK_END);
        if (total_chars > 10000000) { 
            puts("That playlist file is suspiciously big!!!");
            close(file_to_read);
            return;
        }

        /* copy entire file into buffer */
        char* filebuffer = malloc(total_chars+1);
        lseek(file_to_read, 0, SEEK_SET);
        read(file_to_read, filebuffer, total_chars);
        filebuffer[total_chars] = '\0';

        draw_tbar(n, playlist_name);

        /* read line by line*/
        char *linebuffer = malloc(sizeof (*linebuffer) * MAX_HABIT_LEN);
        char *ptr_line = &linebuffer[0];
        char* ptr_file = &filebuffer[0];

        while (*ptr_file != '\0') {
            *ptr_line = *ptr_file;
            ptr_line++;
            ptr_file++;
            if (*(ptr_line - 1) == '\n') {
                *(ptr_line - 1) = '\0';  
                 habit_print(home, linebuffer, n); 
                ptr_line = linebuffer;  
            }
        }

        if (ptr_line != linebuffer) {
            *ptr_line = '\0';
             habit_print(home, linebuffer, n); 
        }

        draw_bbar(n);

        /* free buffers and close file */
        free(linebuffer);
        free(filebuffer);
        close(file_to_read);
    }

    free(playlist_path);
}

void playlist_create(char* playlist_path) {
        int res = file_touch(playlist_path);
        switch (res) {
            case -1:
                puts("Couldn't create file!");
                break;
            case 0:
                puts("Playlist already exists!");
                break;
            case 1:
                puts("Playlist successfully created!");
                break;
        }
}

/* adds to playlist file */
void playlist_add(char* home_path, char* name, char* playlist) {
    if (strlen(name) < 1) return;
    char* playlist_path = alloccat(home_path, playlist);
    int res = file_append(playlist_path, name);
    switch(res) {
    case -1:
        puts("Playlist does not exist.");
        break;
    case 0:
        puts("Could not write for some reason?");
        break;
    case 1:
        printf("%s successfully added to playlist %s.\n", name, playlist );
        break;
    }
    free(playlist_path);
}

/* removes from playlist file */
void playlist_remove(char* home_path, char* name, char* playlist) {
    char* playlist_path = alloccat(home_path, playlist);
    int res = file_delete_line(playlist_path, name);
    free(playlist_path);
    switch (res) {
    case 0:
        printf("Is %s not in playlist %s.\n", name, playlist);
        break;
    case 1:
        printf("%s removed from playlist %s.\n", name, playlist);
        break;
    case -1:
        puts("Could not open playlist file.");
        break;
    }
}

void playlist_delete(char* playlist_path) {
    remove(playlist_path);
}
