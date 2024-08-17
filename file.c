#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "file.h"
#include "config.h"

void red() { printf("\033[1;31m"); }
void blue() { printf("\033[0;34m"); }
void reset() { printf("\033[0m"); }

static const unsigned int NAME_PADDING = 16;

char* alloccat(char* home, char* name) {
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
    char *dup = alloccat(name,"\n");
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

void draw_hseperators(unsigned int n) {
    if (n > MAX_DAYS_PER_LINE) n = MAX_DAYS_PER_LINE;
    if (ASCII_MODE >= 1) {
        LEFT_ELBOW = "|";
        MID_ELBOW = "|";
        RIGHT_ELBOW = "|";
        HBAR = "-";
    }
    printf("%s", LEFT_ELBOW);
    for (register unsigned int i = 0; i < NAME_PADDING+1; i++) printf("%s", HBAR);
    printf("%s", MID_ELBOW);
    for (register unsigned int i = 0; i < (2*n)+1; i++) printf("%s", HBAR);
    printf("%s\n", RIGHT_ELBOW);
}

void draw_tbar(unsigned int n, char* name) {
    if (ASCII_MODE >= 1) {
        TOP_LEFT_CORNER = "|";
        TOP_MID_ELBOW = "|";
        TOP_RIGHT_CORNER = "|";
        HBAR_THICK = "-";
    }
    int nx = n;
    if (n > MAX_DAYS_PER_LINE) nx = MAX_DAYS_PER_LINE;
    printf("%s", TOP_LEFT_CORNER);
    for (register unsigned int i = 0; i < NAME_PADDING+1; i++) printf("%s", HBAR_THICK);
    printf("%s", TOP_MID_ELBOW);
    for (register unsigned int i = 0; i < (2*nx)+1; i++) printf("%s", HBAR_THICK);
    printf("%s\n", TOP_RIGHT_CORNER);
    if (n>(nx+1)) {
        char rows[10];
        sprintf(rows, "%d",  (n-(n%nx))/nx );
        int backrows = strlen(rows);
        if (n%MAX_DAYS_PER_LINE==0) {
            printf("%s%*s %s %*sx%d days %s\n", VBAR_THICK, NAME_PADDING, name, VBAR_THIN, (2*nx)-8-(backrows), rows, nx, VBAR_THICK);
        }
        else {
            char rem[10];
            sprintf(rem, "%d",  n%nx);
            int backrem = strlen(rem);
            printf("%s%*s %s %*s+%sx%d days %s\n", VBAR_THICK, NAME_PADDING, name, VBAR_THIN, (2*nx)-9-(backrows)-(backrem), rem, rows, nx, VBAR_THICK);

        }
    }
    else printf("%s%*s %s %*d days %s\n", VBAR_THICK, NAME_PADDING, name, VBAR_THIN, (2*nx)-4-2, n, VBAR_THICK);
}

void draw_bbar(unsigned int n) {
    if (ASCII_MODE >= 1) {
        BOTTOM_LEFT_CORNER = "|";
        BOTTOM_MID_ELBOW = "|";
        BOTTOM_RIGHT_CORNER = "|";
        HBAR_THICK = "-";
    }
    if (n > MAX_DAYS_PER_LINE) n = MAX_DAYS_PER_LINE;
    printf("%s", BOTTOM_LEFT_CORNER);
    for (register unsigned int i = 0; i < NAME_PADDING+1; i++) printf("%s", HBAR_THICK);
    printf("%s", BOTTOM_MID_ELBOW);
    for (register unsigned int i = 0; i < (2*n)+1; i++) printf("%s", HBAR_THICK);
    printf("%s\n", BOTTOM_RIGHT_CORNER);
}

/* most important and basic function */
void print_habitn_from_file(char* path, char* name, unsigned int n) {
    if (n > 50000) n = 50000;
    if (ASCII_MODE >= 1) {
        LEFT_ELBOW = "|";
        MID_ELBOW = "|";
        RIGHT_ELBOW = "|";
        HBAR = "-";
        VBAR_THIN = "|";
        VBAR_THICK = "|";
    }

    FILE * fread;
    char* habitpath = alloccat(path, name);
    fread = fopen(habitpath, "r");
    if (fread == NULL){
        printf("Habit %s does not exist! (%s)\n", name, habitpath);
    } else {
        unsigned int c = 0;
        time_t right;
        time_t left = get_now_date()+(24*60*60);

        /* print habit separator */
        if (SEPERATE_HABITS) draw_hseperators(n);
        printf("%s%*s %s ", VBAR_THICK, 16, name, VBAR_THIN);
        char buffer[31]; 

        /* Print marked dates using left and right pointers */
        while (NULL != fgets(buffer, BUF_SIZE, fread)) {
            right = strtoul(buffer, NULL, 10); 
            int in_between = (difftime(left, right) / (24*60*60))-1;
            if (in_between >  0) {
                for (register int i = 0; i < in_between; i++) {
                    if (c >= n) break;
                    blue();
                    printf("o ");
                    reset();
                    c++;
                    if (c%MAX_DAYS_PER_LINE == 0 && c!=n)
                        printf("%s\n%s\t\t  %s ", VBAR_THICK, VBAR_THICK, VBAR_THIN);
                }
                if (c >= n) break;
            }
            red();
            printf("x ");
            c++;
            reset();
            if (c%MAX_DAYS_PER_LINE == 0 && c!=n )
                printf("%s\n%s\t\t  %s ", VBAR_THICK, VBAR_THICK, VBAR_THIN);
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
                if (c%MAX_DAYS_PER_LINE == 0 && c!=n )
                        printf("%s\n%s\t\t  %s ", VBAR_THICK, VBAR_THICK, VBAR_THIN);
            }
        }
        /* fill in the space in the remaining row */
        int nx = n;
        if (n > MAX_DAYS_PER_LINE) nx = MAX_DAYS_PER_LINE;
        if (c%MAX_DAYS_PER_LINE) printf("%*s", 2*(nx - c%MAX_DAYS_PER_LINE), "");
        puts(VBAR_THICK);
        fclose(fread);
    }
    free(habitpath);
    if (n>50000) {
        printf("The max number of days ago"); red(); printf(" shown "); reset(); puts("is 50000 days (aprox. 136 years)\n Excuse me immortal vampires and time travelers.");
    }
}

/* prints habits from a dot playlist file */
void print_habitsn_from_playlist_file(char* path, char* playlist, unsigned int n) {

    char* pl_path = alloccat(path, playlist);
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
    char* playlist_path = alloccat(home, playlist);
    FILE* fadd = fopen(playlist_path, "a");
    fprintf(fadd, "%s\n", name); 
    fclose(fadd);
    free(playlist_path);
}

void remove_from_playlist(char* path, char* name, char* playlist) {
    char* rpath = alloccat(path, playlist);
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
    char* realpath = alloccat(path, name);
    FILE* fread = (fopen(realpath, "r"));
    if( fread == NULL ) {
        puts("Habit doesn't exist!");
    } else {
        char buf[BUF_SIZE];
        fgets(buf, BUF_SIZE, fread);
        time_t line1 =  strtoul(buf, NULL, 10);

        if (get_now_date() <= line1 ) {
            puts("Today already marked!");
        }
        else {
            char *tempfilename = "___temporary file___";
            FILE* fwrite = fopen(tempfilename, "w");
            fprintf(fwrite, "%ld\n", get_now_date());
            rewind(fread);
            while (NULL != fgets(buf, BUF_SIZE, fread)) {
                fputs(buf, fwrite); 
            }
            fclose(fwrite);
            remove(realpath);
            rename(tempfilename, realpath);
        }
        fclose(fread);
    }
    free(realpath);
}
