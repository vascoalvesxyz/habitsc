#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "file.h"

#define BUF_SIZE 31

void red() { printf("\033[1;31m"); }
void blue() { printf("\033[0;34m"); }
void reset() { printf("\033[0m"); }

char* alloc_real_path(char* home, char* name) {
    char* realname = (char*) malloc(sizeof(char)*(strlen(home)+strlen(name) + 1));
    strcpy(realname, home);
    strcat(realname, name);
    return realname;
}

time_t make_date(time_t time) {
        struct tm *lnow = localtime(&time);
        lnow->tm_min = 0;
        lnow->tm_sec = 0;
        lnow->tm_hour = 0;
        return mktime(lnow);
}

time_t get_now_date() {
    return make_date(time(NULL));
}

int search_file(FILE* f, char* name ) {
    if (f == NULL) return 0; /* can't search an empty file */
    char *dup = alloc_real_path(name,"\n");
    char buf[BUF_SIZE];
    int res = 0;
    while (NULL != fgets(buf, BUF_SIZE, f)) {
        if (strcmp(buf, dup) == 0) {
            res = 1;
            break;
        }
    }
    free(dup);
    return res;
}


/* most important and basic function */
void print_habitn_from_file(char* path, char* name, int n) {
    if (n > 50000) n = 50000;

    FILE * fread;
    char* habitpath = alloc_real_path(path, name);
    fread = fopen(habitpath, "r");
    if (fread == NULL){
        printf("Habit %s does not exist! (%s)\n", name, habitpath);
    } else {
        int c = 0;
        time_t right = 0;
        time_t left = get_now_date();

        printf("%10s | ", name);
        char buffer[31]; /* names shouldn't be longer than 30 char */
        while (EOF != fscanf(fread, "%[^\n]\n", buffer)) {
            right = strtoul(buffer, NULL, 10); 
            /* check how many days there are between left and right */
            int in_between = (difftime(left, right) / (24*60*60)) - 1;
            if (in_between >  0) {
                for (int i = 0; i < in_between; i++) {
                    if (c >= n) break;
                    blue();
                    printf("o ");
                    reset();
                    c++;
                    if (c%30 == 0 ) printf("\n\t   | ");
                }
                if (c >= n) break;
            }
            /* print marked date */
            red();
            printf("x ");
            c++;
            reset();
            if (c%30 == 0 ) printf("\n\t   | ");
            if (c >= n) break;

            left = right;
        }

        /* if there are still characters to fill */
        if (c < n) {
            while (c < n) {
                blue();
                printf("o ");
                reset();
                c++;
                if (c%30 == 0 ) printf("\n\t   | ");
            }
        }
        printf("\n");
        fclose(fread);
    }
    free(habitpath);
    if (n>50000)
    {
        printf("The max number of days ago");
        red();
        printf(" shown ");
        reset();
        printf("is 50000 days (aprox. 136 years)\n Excuse me if you are a time traveler.\n");
    }
}

/* prints habits from a dot playlist file */
void print_habitsn_from_playlist_file(char* path, char* playlist, int n) {
    char* pl_path = alloc_real_path(path, playlist);
    FILE* fptr = fopen(pl_path, "r");

    if(fptr==NULL) {
        printf("Playlist not found!");
    }
    else {
        char buffer[31]; /* names shouldn't be longer than 30 char */
        while (EOF != fscanf(fptr, "%[^\n]\n", buffer)) {
            print_habitn_from_file(path, buffer, n);
        }
        fclose(fptr);
    }

    free(pl_path);
}

/* adds to playlist file */
void add_to_playlist(char* home, char* name, char* playlist) {
    if (strlen(name) > 10) {
        printf("Names can't be longer than 10 characters!\n");
        return;
    }
    char* playlist_path = alloc_real_path(home, playlist);
    FILE* fadd = fopen(playlist_path, "a");
    fprintf(fadd, "%s\n", name); 
    fclose(fadd);
    free(playlist_path);
}

void remove_from_playlist(char* path, char* name, char* playlist) {
    char* rpath = alloc_real_path(path, playlist);
    FILE* fplaylist = fopen(rpath, "r");
    printf("Removing %s from %s\n", name, rpath);
    if (fplaylist == NULL) {
        printf("Playlist does not exist!");
    }
    else {
        char *tempfilename = "___temporary file___";
        FILE* temp = fopen(tempfilename, "w");
        strcat(name, "\n"); /* must add newline to compare */
        char *buffer = calloc(31, sizeof(char));
        while (NULL != fgets(buffer, 30, fplaylist)) { /* find line to skip */
            if (strcmp(name, buffer)) fputs(buffer, temp);
        }
        fclose(temp);
        free(buffer);

        remove(rpath);
        rename(tempfilename, rpath);
    }
    fclose(fplaylist);
}

void mark_in_file(char* path, char* name) {
    char* realpath = (char*) malloc(sizeof(char)*(strlen(path)+strlen(name)+1));
    strcpy(realpath, path);
    strcat(realpath, name);

    FILE* fread = (fopen(realpath, "r"));
    if( fread != NULL ) 
    {
        char buf[BUF_SIZE];
        fgets(buf, BUF_SIZE, fread);
        time_t line1 =  strtoul(buf, NULL, 10);

        if (get_now_date() > line1 ) {
            char *tempfilename = "___temporary file___";
            FILE* fwrite = fopen(tempfilename, "w");
            fprintf(fwrite, "%ld\n", get_now_date());
            rewind(fread);
            while (NULL != fgets(buf, BUF_SIZE, fread)) fputs(buf, fwrite); 
            fclose(fread);
            fclose(fwrite);
            remove(realpath);
            rename(tempfilename, realpath);
        } else {
            printf("Today already marked!\n");
        }
        if (fread) fclose(fread);
    } else {
        printf("Habit doesn't exist!\n");
    }
    free(realpath);
}
