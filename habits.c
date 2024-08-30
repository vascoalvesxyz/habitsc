#include "config.h"
#include "habits.h"
#include <fcntl.h>
#include <stdio.h>

static const unsigned int LINEBUFFER = 32;
static const unsigned int NAME_PADDING = 16;

/* functions */
time_t date_midnight(time_t time);

void habit_mark(char* habit_path); 
int  habit_create(char* habit_path); 
int  habit_delete(char* habit_path);
void habit_rename(char* path, char* old_name, char* new_name);
void habit_print(char* home, char* habit_name, unsigned int total_days); 

void playlist_create(char* playlist_path);
void playlist_delete(char* playlist_path); 
void playlist_add(char* home_path, char* name, char* playlist); 
void playlist_remove(char* home_path, char* name, char* playlist); 
void playlist_print(char* home, char* playlist_name, unsigned int n); 

void red();
void blue();
void reset();
void enable_ascci();
void draw_tbar(unsigned int n, char* name);
void draw_bbar(unsigned int n);
void draw_hseperators(unsigned int n);

/* implementations */
void red() {
    printf("\033[1;31m");
}
void blue() {
    printf("\033[0;34m");
}
void reset() {
    printf("\033[0m");
}

void enable_ascci() {
    LEFT_ELBOW  = "|";
    RIGHT_ELBOW = "|";
    MID_ELBOW   = "+";
    HBAR        = "-";
    HBAR_THICK  = "=";
    VBAR_THIN   = "|";
    VBAR_THICK  = "|";
    TOP_LEFT_CORNER     = "+";
    TOP_MID_ELBOW       = "+";
    TOP_RIGHT_CORNER    = "+";
    BOTTOM_LEFT_CORNER  = "+";
    BOTTOM_MID_ELBOW    = "+";
    BOTTOM_RIGHT_CORNER = "+";
}

void habit_print(char* home, char* habit_name, unsigned int total_days) {
    FILE * file_to_Read;
    char* habitpath = alloccat(home, habit_name);
    file_to_Read = fopen(habitpath, "r");
    if (file_to_Read == NULL){
        printf("Habit %s does not exist! (%s)\n", habit_name, habitpath);
    } else {
        unsigned int c = 0;
        time_t right;
        time_t left = date_midnight(time(NULL))+(24*60*60);

        if (SEPERATE_HABITS == 1) draw_hseperators(total_days);

        printf("%s%*s %s ", VBAR_THICK, 16, habit_name, VBAR_THIN);
        char buffer[LINEBUFFER]; 

        /* Print marked dates using left and right pointers */
        while (NULL != fgets(buffer, LINEBUFFER, file_to_Read)) {
            right = strtoul(buffer, NULL, 10); 

            /* Print difference in between current marked date and next one*/
            int in_between = (difftime(left, right) / (24*60*60))-1;
            if (in_between >  0) {
                for (register int i = 0; i < in_between; i++) {
                    if (c >= total_days) break; /* break for loop */
                    blue();
                    printf("o ");
                    reset();
                    c++;
                    if (c % MAX_DAYS_PER_LINE == 0 && c!=total_days) {
                        printf("%s\n%s\t\t  %s ", VBAR_THICK, VBAR_THICK, VBAR_THIN);
                    }
                }
                if (c >= total_days) break; /* break while loop */
            }

            /* Print marked date */
            red();
            printf("x ");
            c++;
            reset();
            if (c%MAX_DAYS_PER_LINE == 0 && c!= total_days )
                printf("%s\n%s\t\t  %s ", VBAR_THICK, VBAR_THICK, VBAR_THIN);
            if (c >= total_days) break;
            left = right;
        }

        /* if there are still characters to fill */
        if (c < total_days) {
            while (c < total_days) {
                blue();
                printf("o ");
                reset();
                c++;
                if (c%MAX_DAYS_PER_LINE == 0 && c!=total_days )
                        printf("%s\n%s\t\t  %s ", VBAR_THICK, VBAR_THICK, VBAR_THIN);
            }
        }

        /* fill in the white space in the remaining row */
        int nx = total_days;
        if (total_days > MAX_DAYS_PER_LINE) nx = MAX_DAYS_PER_LINE;
        if (c%MAX_DAYS_PER_LINE) printf("%*s", 2*(nx - c%MAX_DAYS_PER_LINE), "");
        printf("%s\n", VBAR_THICK);
        fclose(file_to_Read);
    }

    free(habitpath);
}

/* prints habits from a dot playlist file */
void playlist_print(char* home, char* playlist_name, unsigned int n) {

    if (n < 7) n = 30;

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
        char filebuffer[total_chars+1];
        lseek(file_to_read, 0, SEEK_SET);
        read(file_to_read, filebuffer, total_chars);
        filebuffer[total_chars] = '\0';

        draw_tbar(n, playlist_name);

        /* read line by line*/
        char linebuffer[MAX_HABIT_LEN];
        char *ptr_line = linebuffer;
        char *ptr_file = filebuffer;

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
        close(file_to_read);

        if (n > 50000) {
            printf("The number of days ago");
            red();
            printf(" EXCEEDS 50000 DAYS ");
            reset();
            puts("(aprox. 136 years)\n Hello immortal vampires and time travelers!");
        }

    }

    free(playlist_path);
}

void draw_hseperators(unsigned int n) {
    if (n > MAX_DAYS_PER_LINE) n = MAX_DAYS_PER_LINE;
    printf("%s", LEFT_ELBOW);
    for (register unsigned int i = 0; i < NAME_PADDING+1; i++) printf("%s", HBAR);
    printf("%s", MID_ELBOW);
    for (register unsigned int i = 0; i < (2*n)+1; i++) printf("%s", HBAR);
    printf("%s\n", RIGHT_ELBOW);
}

void draw_tbar(unsigned int n, char* name) {
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
    if (n > MAX_DAYS_PER_LINE) n = MAX_DAYS_PER_LINE;
    printf("%s", BOTTOM_LEFT_CORNER);
    for (register unsigned int i = 0; i < NAME_PADDING+1; i++) printf("%s", HBAR_THICK);
    printf("%s", BOTTOM_MID_ELBOW);
    for (register unsigned int i = 0; i < (2*n)+1; i++) printf("%s", HBAR_THICK);
    printf("%s\n", BOTTOM_RIGHT_CORNER);
}

time_t
date_midnight(time_t time)
{
        struct tm *lnow = localtime(&time);
        lnow->tm_min = 0;
        lnow->tm_sec = 0;
        lnow->tm_hour = 0;
        return mktime(lnow);
}


void
habit_mark(char* habit_path)
{
    /* check last date */
    FILE* file_to_read = fopen(habit_path, "r");
    unsigned long first_line = 0;
    if (file_to_read == NULL) {
        puts("Habit does not exist!");
        return;
    } else {
        char buf[32];
        if (fgets(buf, sizeof(buf), file_to_read) != NULL) {
            char* endptr;
            first_line = strtoul(buf, &endptr, 10);
            if (endptr == buf || (*endptr != '\n' && *endptr != '\0')) {
                fclose(file_to_read);
                puts("File is empty!");
                return;
            }
        }
        fclose(file_to_read);
    }

    /* add current date to file */
    time_t current_midnight = date_midnight(time(NULL));
    if (current_midnight <= (time_t) first_line) {
        puts("Today already marked!");
    } else {
        char buf[32];
        sprintf(buf, "%ld", (long)current_midnight);
        file_prepend(habit_path, buf);
    }
}

int
habit_create(char* habit_path)
{
        int res = file_touch(habit_path);
        switch (res) {
            case ERR_NOFILE:
                puts("Failed to create file!");
                break;
            case FALSE:
                puts("Habit already exists!");
                break;
            case TRUE:
                puts("Habit successfully created!");
                break;
        }
        return res;
}

int
habit_delete(char *habit_path)
{
    int fhabit = open(habit_path, O_RDONLY);
    if (fhabit == ERR_NOFILE) {
        return ERR_NOFILE;
    } else {
        char input;
        printf("\tAre you sure you want to delete this habit forever? (y/N)");
        scanf("%c", &input);
        if (input != 'y' ) {
            close(fhabit);
            return FALSE;
        } else {
            remove(habit_path);
        }
    }
    close(fhabit);
    return TRUE;
}

void
playlist_create(char* playlist_path)
{
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

void
playlist_add(char* home_path, char* name, char* playlist)
{
    if (strlen(name) < 1) return;
    char* playlist_path = alloccat(home_path, playlist);

    int res = file_append(playlist_path, name);

    switch(res) {
    case ERR_NOFILE:
        puts("Playlist does not exist.");
        break;
    case FALSE:
        puts("Could not write for some reason?");
        break;
    case TRUE:
        printf("%s successfully added to playlist %s.\n", name, playlist );
        break;
    }

    free(playlist_path);
}

void
playlist_remove(char* home_path, char* name, char* playlist)
{
    char* playlist_path = alloccat(home_path, playlist);
    int res = file_delete_line(playlist_path, name);
    free(playlist_path);

    switch (res) {
    case ERR_NOFILE:
        puts("Could not open playlist file.");
        break;
    case ERR_NOTFOUND:
        printf("%s not found in playlist %s.\n", name, playlist);
        break;
    case TRUE:
        printf("%s removed from playlist %s.\n", name, playlist);
        break;
    }
}

void
playlist_delete(char* playlist_path)
{
    remove(playlist_path);
}

int
main(int argc, char *argv[])
{
    char* home_path = alloccat(getenv("HOME"),  "/.local/share/habits/");

    /* touch all.playlist */
    char* allpath = alloccat(home_path, ALL);
    file_touch(allpath);
    free(allpath);

    if (ASCII_MODE == TRUE)
        enable_ascci();

    /* if only one argument, print n days */
    uint days = DEFAULT_DAYS;
    if (argc <= 2) {
        if (argv[1] != NULL)
            days = strtod(argv[1], NULL) ;
        playlist_print(home_path, ALL, days);
        free(home_path);
        return 0;
    }

    char* habit_path = alloccat(home_path, argv[2]);
    if (argv[1][1] == 'h') {

        switch (argv[1][2]) {

        case 'a':
            if (strlen(argv[2]) > 10) {
                puts("That name is too long.");
                break;
            }
            if (habit_create(habit_path) == TRUE) {
                playlist_add(home_path, argv[2], ALL);
                playlist_print(home_path, ALL, days);
            }
            else {
                puts("Habit does not exist!");
            }
            break;
        case 'd':
            switch (habit_delete(habit_path)) {
            case -1:
                puts("The habit does not exist!");
                break;
            case TRUE:
                playlist_remove(home_path, argv[2], ALL);
                /*playlist_print(home_path, ALL, days);*/
                break;
            default:
                puts("Bruh");
                break;
            }
            break;
        case 'm':
            habit_mark(habit_path);
            playlist_print(home_path, ALL, days);
            break;

        case 's':
            if (argv[3] != NULL) days = strtod(argv[3], NULL);
            draw_tbar(days, "STATUS");
            habit_print(home_path, argv[2], days);
            draw_bbar(days);
            break;

        default:
            puts("Not a valid flag!");
            break;
        } /* end of switch */ 

    free(habit_path);

    } /* end of if */
    else if (argv[1][1] == 'p' && argv[2]) {
        int days = DEFAULT_DAYS;

        if (strcmp(argv[2], "all") == 0) {
            puts("You can't use that playlist name!");
        } 

        char* plname = alloccat(argv[2], ".playlist");
        char* plpath = alloccat(home_path, plname);

        switch (argv[1][2]) {

        case 'c':
            if (argv[2] == NULL) break;
            playlist_create(plpath);
            break;

        case 's':
            if (argv[3]) days = strtod(argv[3], NULL);
            playlist_print(home_path, plname, days);
            break;

        case 'a':
            if (argv[3] == NULL) break;
            playlist_add(home_path, argv[3] , plname);
            playlist_print(home_path, plname, days);
            break;

        case 'r':
            if (argv[3] == NULL) break;
            playlist_remove(home_path, argv[3] , plname);
            playlist_print(home_path, plname, days);
            break;

        case 'd':
            playlist_delete(plpath);
            playlist_print(home_path, plname, days);
            break;

        default:
            puts("Not a valid flag!");
            break;

    } /* end of case p */
    free(plpath);
    free(plname);
    } /* end of else if */
    else {
        puts("Not a valid flag!");
    }
    free(home_path);
    return 0;
} /* end of main */
