#include "habits.h"
#include "config.h"

/*static const unsigned int LINEBUFFER = 32;*/
static const unsigned int NAME_PADDING = 16;


/* implementations */
void red() { printf("\033[1;31m"); }
void blue() { printf("\033[0;34m"); }
void reset() { printf("\033[0m"); }
/* functions */
void blue();
void red();
void reset();
time_t date_midnight(time_t time);
void draw_tbar(unsigned int n, char *name);
void draw_bbar(unsigned int n);
void draw_hseperators(unsigned int n);
void enable_ascci();
int  habit_create(char *habit_path);
int  habit_delete(char *habit_path);
void habit_insert(char *habit_path, char* date, int is_date);
void habit_print(char *home, char *habit_name, unsigned int total_days);
void habit_print_dates(char *home, char *habit_name, unsigned int total_days);
void habit_rename(char *path, char *old_name, char *new_name);
void playlist_add(char *home_path, char *name, char *playlist);
void playlist_create(char *playlist_path);
void playlist_delete(char *playlist_path);
void playlist_print(char *home, char *playlist_name, unsigned int n);
void playlist_remove(char *home_path, char *name, char *playlist);

void enable_ascci() {
    LEFT_ELBOW = "|";
    RIGHT_ELBOW = "|";
    MID_ELBOW = "+";
    HBAR = "-";
    HBAR_THICK = "=";
    VBAR_THIN = "|";
    VBAR_THICK = "|";
    TOP_LEFT_CORNER = "+";
    TOP_MID_ELBOW = "+";
    TOP_RIGHT_CORNER = "+";
    BOTTOM_LEFT_CORNER = "+";
    BOTTOM_MID_ELBOW = "+";
    BOTTOM_RIGHT_CORNER = "+";
}

void
habit_print(char *home, char *habit_name, unsigned int total_days) 
{
    /* tldr: read file to buffer the size of a sector,
     * iterate strings in array via pointers,
     * habits are prepended by the way, shouldn't cause problems
     * for strangely big files 
     * after that is a simple loop where you calculate the difference between dates */

    char *habitpath = alloccat(home, habit_name);
    int file_to_read = open(habitpath, O_RDONLY);

    if (file_to_read == ERR_NOFILE) {
        printf("Habit %s does not exist! (%s)\n", habit_name, habitpath);
        free(habitpath);
        return;
    }

    char buffer[BUFSIZ+1];

    int n_bytes = lseek(file_to_read, 0, SEEK_END);
    lseek(file_to_read,  0, SEEK_SET);
    read(file_to_read, buffer, n_bytes);

    if (SEPERATE_HABITS == 1)
        draw_hseperators(total_days);

    printf("%s%*s %s ", VBAR_THICK, 16, habit_name, VBAR_THIN);

    uint i = 0, c = 0;

    /* if the file is not empty, it will iterate from left to right */
    if (n_bytes != 0) {
        if (n_bytes < BUFSIZ)  {
            *(buffer+n_bytes-1) = '\n';
            *(buffer+n_bytes) = '\0';
        }

        unsigned long left = date_midnight(time(NULL)) + 24*60*60;

        char *bufptr = &buffer[0];
        char *endptr = strchr(bufptr, '\n');
        *(endptr) = '\0';

        unsigned long right = strtoul(bufptr, NULL, 10);
        int in_between = 0; /* needs to be an int to avoid overflow */

        while (*bufptr != '\0')
        {
            in_between = (difftime(left, right) / (24 * 60 * 60)) - 1;

            if (in_between > 0) {

                for (i = 0; i < in_between; i++)
                {
                    if (c >= total_days)
                        break; 
                    blue();
                    printf("o ");
                    reset();
                    c++;
                    if (c % MAX_DAYS_PER_LINE == 0 && c != total_days) {
                        printf("%s\n%s\t\t  %s ", VBAR_THICK, VBAR_THICK, VBAR_THIN);
                    }
                }
                if (c >= total_days)
                    break; 
            }

            red();
            printf("x ");
            c++;
            reset();

            if (c % MAX_DAYS_PER_LINE == 0 && c != total_days)
                printf("%s\n%s\t\t  %s ", VBAR_THICK, VBAR_THICK, VBAR_THIN);
            if (c >= total_days)
                break;

            left = right;
            bufptr = endptr + 1;
            if ( NULL != (endptr = strchr(bufptr, '\n')) ) {
                *(endptr) = '\0';
            }
            right = strtoul(bufptr, NULL, 10);
        }
    }

    /* print remaining days */
    if (c < total_days) {
        while (c < total_days) {
            blue();
            printf("o ");
            reset();
            c++;
            if (c % MAX_DAYS_PER_LINE == 0 && c != total_days)
                printf("%s\n%s\t\t  %s ", VBAR_THICK, VBAR_THICK, VBAR_THIN);
        }
    }

    int nx = total_days;
    if (total_days > MAX_DAYS_PER_LINE)
        nx = MAX_DAYS_PER_LINE;
    if (c % MAX_DAYS_PER_LINE)
        printf("%*s", 2 * (nx - c % MAX_DAYS_PER_LINE), "");
    printf("%s\n", VBAR_THICK);
    close(file_to_read);

    free(habitpath);
}

void
habit_print_dates(char *home, char *habit_name, unsigned int total_days) 
{
    /* tldr: habit_print but prints dates in a column
     * instead of x's and o's in rows
     * mostly for debugging
     * */

    char habitpath[128];
    strcpy(habitpath, home);
    strcat(habitpath, habit_name);

    int file_to_read = open(habitpath, O_RDONLY);
    if (file_to_read == ERR_NOFILE) {
        printf("Habit %s does not exist! (%s)\n", habit_name, habitpath);
        return;
    }

    int n_bytes = lseek(file_to_read, 0, SEEK_END);
    lseek(file_to_read,  0, SEEK_SET);

    char buffer[n_bytes+1];
    read(file_to_read, buffer, n_bytes);

    *(buffer+n_bytes-1) = '\n';
    *(buffer+n_bytes) = '\0';

    unsigned long left = date_midnight(time(NULL));

    char *bufptr = &buffer[0];
    char *endptr = strchr(bufptr, '\n');
    *(endptr) = '\0';

    unsigned long right = strtoul(bufptr, NULL, 10);
    int in_between;

    struct tm *lt;
    while (*bufptr != '\0')
    {
        in_between = (difftime(left, right) / (24 * 60 * 60)) - 1;
        lt = localtime( (time_t*) &left ); 
        printf("| %02d/%02d/%04d - %02d - ", lt->tm_mday, lt->tm_mon+1, lt->tm_year+1900, in_between);
        lt = localtime( (time_t*) &right ); 
        printf("%02d/%02d/%04d |\n", lt->tm_mday, lt->tm_mon+1, lt->tm_year+1900);

        left = right;
        bufptr = endptr + 1;
        if ( NULL != (endptr = strchr(bufptr, '\n')) ) {
                *(endptr) = '\0';
        }
        right = strtoul(bufptr, NULL, 10);
    }
    lt = localtime( (time_t*) &right); 

    close(file_to_read);
}


void playlist_print(char *home, char *playlist_name, unsigned int n) {

    if (n < 7)
        n = 30;

    char *playlist_path = alloccat(home, playlist_name);
    int file_to_read = open(playlist_path, O_RDONLY);

    if (file_to_read == -1) {
        printf("Playlist not found!");
    } else {

        int total_chars = lseek(file_to_read, 0, SEEK_END);
        if (total_chars > 10000000) {
            puts("That playlist file is suspiciously big!!!");
            close(file_to_read);
            return;
        }

        char filebuffer[total_chars + 1];
        lseek(file_to_read, 0, SEEK_SET);
        read(file_to_read, filebuffer, total_chars);
        filebuffer[total_chars] = '\0';

        draw_tbar(n, playlist_name);

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
    if (n > MAX_DAYS_PER_LINE)
        n = MAX_DAYS_PER_LINE;
    printf("%s", LEFT_ELBOW);
    for (register unsigned int i = 0; i < NAME_PADDING + 1; i++)
        printf("%s", HBAR);
    printf("%s", MID_ELBOW);
    for (register unsigned int i = 0; i < (2 * n) + 1; i++)
        printf("%s", HBAR);
    printf("%s\n", RIGHT_ELBOW);
}

void draw_tbar(unsigned int n, char *name) {
    int nx = n;

    if (n > MAX_DAYS_PER_LINE)
        nx = MAX_DAYS_PER_LINE;
    printf("%s", TOP_LEFT_CORNER);
    for (register unsigned int i = 0; i < NAME_PADDING + 1; i++)
        printf("%s", HBAR_THICK);
    printf("%s", TOP_MID_ELBOW);
    for (register unsigned int i = 0; i < (2 * nx) + 1; i++)
        printf("%s", HBAR_THICK);
    printf("%s\n", TOP_RIGHT_CORNER);

    if (n > (nx + 1)) {
        char rows[10];
        sprintf(rows, "%d", (n - (n % nx)) / nx);
        int backrows = strlen(rows);
        if (n % MAX_DAYS_PER_LINE == 0) {
            printf("%s%*s %s %*sx%d days %s\n", VBAR_THICK, NAME_PADDING, name,
                    VBAR_THIN, (2 * nx) - 8 - (backrows), rows, nx, VBAR_THICK);
        } else {
            char rem[10];
            sprintf(rem, "%d", n % nx);
            int backrem = strlen(rem);
            printf("%s%*s %s %*s+%sx%d days %s\n", VBAR_THICK, NAME_PADDING, name,
                    VBAR_THIN, (2 * nx) - 9 - (backrows) - (backrem), rem, rows, nx,
                    VBAR_THICK);
        }
    } else
        printf("%s%*s %s %*d days %s\n", VBAR_THICK, NAME_PADDING, name, VBAR_THIN,
                (2 * nx) - 4 - 2, n, VBAR_THICK);
}

void draw_bbar(unsigned int n) {
    if (n > MAX_DAYS_PER_LINE)
        n = MAX_DAYS_PER_LINE;
    printf("%s", BOTTOM_LEFT_CORNER);
    for (register unsigned int i = 0; i < NAME_PADDING + 1; i++)
        printf("%s", HBAR_THICK);
    printf("%s", BOTTOM_MID_ELBOW);
    for (register unsigned int i = 0; i < (2 * n) + 1; i++)
        printf("%s", HBAR_THICK);
    printf("%s\n", BOTTOM_RIGHT_CORNER);
}

time_t date_midnight(time_t time) {
    struct tm *lnow = localtime(&time);
    lnow->tm_min = 0;
    lnow->tm_sec = 0;
    lnow->tm_hour = 0;
    return mktime(lnow);
}

int habit_create(char *habit_path) {
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

int habit_delete(char *habit_path) {
    int fhabit = open(habit_path, O_RDONLY);
    if (fhabit == ERR_NOFILE) {
        return ERR_NOFILE;
    } else {
        char input;
        printf("\tAre you sure you want to delete this habit forever? (y/N)");
        scanf("%c", &input);
        if (input != 'y') {
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
habit_insert(char *habit_path, char *date, int is_date) 
{
    int file_to_read = open(habit_path, O_RDONLY);

    if (file_to_read == ERR_NOFILE) {
        puts("Habit does not exist!");
        return;
    }

    char buffer[BUFSIZ];
    int n_bytes = lseek(file_to_read, 0, SEEK_END);
    lseek(file_to_read, 0, SEEK_SET);

    if (n_bytes >= BUFSIZ) {
        n_bytes = BUFSIZ - 1;
        buffer[BUFSIZ-1] = '\0';
    }

    read(file_to_read, buffer, n_bytes);

    time_t time_where;

    if (is_date == FALSE) {
        time_where = date_midnight(time(NULL)) - strtod(date, NULL)*24*60*60;
    } else {
        struct tm d;
        sscanf(date, "%d/%d/%d", &d.tm_mday, &d.tm_mon, &d.tm_year );
        d.tm_year -= 1900;
        d.tm_mon -= 1;
        time_where = mktime(&d);
    }

    if (time_where > date_midnight(time(NULL))) {
        close(file_to_read);
        puts("Time traveler huh?");
        return;
    }

    char* bufptr = buffer;
    char* endptr;
    time_t time_read = -1;

    while ( (endptr = strchr(bufptr, '\n')) != NULL ) {
        *endptr = '\0';
        time_read = strtoul(bufptr, NULL, 10);
        *endptr = '\n';
        if (time_read <= time_where) 
            break;
        bufptr = endptr + 1;
    }

    char buf[64];
    sprintf(buf, "%ld\n" , time_where); 

    if (time_read == time_where) {
        file_delete_line(habit_path, buf);
        return;
    }

    char *temppath = alloccat(habit_path, "___temp___");
    int file_to_write = open(temppath, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    write(file_to_write, buffer, bufptr - buffer);
    write(file_to_write, buf, strlen(buf));
    write(file_to_write, bufptr, strlen(bufptr));
    remove(habit_path);
    rename(temppath, habit_path);
    free(temppath);
    close(file_to_write);
}

void playlist_create(char *playlist_path) {
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

void playlist_add(char *home_path, char *name, char *playlist) {
    if (strlen(name) < 1)
        return;
    char *playlist_path = alloccat(home_path, playlist);

    int res = file_append(playlist_path, name);

    switch (res) {
        case ERR_NOFILE:
            puts("Playlist does not exist.");
            break;
        case FALSE:
            puts("Could not write for some reason?");
            break;
        case TRUE:
            printf("%s successfully added to playlist %s.\n", name, playlist);
            break;
    }

    free(playlist_path);
}

void playlist_remove(char *home_path, char *name, char *playlist) {
    char *playlist_path = alloccat(home_path, playlist);
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

void playlist_delete(char *playlist_path) { remove(playlist_path); }

int main(int argc, char *argv[]) {

    char *home_path = alloccat(getenv("HOME"), "/.local/share/habits/");

    char *allpath = alloccat(home_path, ALL);
    file_touch(allpath);
    free(allpath);

    if (ASCII_MODE == TRUE)
        enable_ascci();

    uint days = DEFAULT_DAYS;
    if (argc <= 2) {
        if (argv[1] != NULL)
            days = strtod(argv[1], NULL);
        playlist_print(home_path, ALL, days);
        free(home_path);
        return 0;
    }


    char *habit_path = alloccat(home_path, argv[2]);
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
                } else {
                    puts("Habit does not exist!");
                }
                break;
            case 'i':
                /* here, days is days going back */
                if (argv[3] != NULL) {
                    habit_insert(habit_path, argv[3], FALSE);
                    playlist_print(home_path, ALL, DEFAULT_DAYS);
                }
                else {
                    puts("Provide a number of days ago to insert at.");
                }
                break;
            case 'I':
                if (argv[3] != NULL) {
                    habit_insert(habit_path, argv[3], TRUE);
                    playlist_print(home_path, ALL, DEFAULT_DAYS);
                }
                else {
                    puts("Provide a date to insert at.");
                }
                break;
            case 'd':
                switch (habit_delete(habit_path)) {
                    case -1:
                        puts("The habit does not exist!");
                        break;
                    case TRUE:
                        playlist_remove(home_path, argv[2], ALL);
                        break;
                    default:
                        puts("Bruh");
                        break;
                }
                break;
            case 'm':
                habit_insert(habit_path, "0", FALSE);
                playlist_print(home_path, ALL, days);
                break;

            case 's':
                if (argv[3] != NULL)
                    days = strtod(argv[3], NULL);
                draw_tbar(days, "STATUS");
                habit_print(home_path, argv[2], days);
                draw_bbar(days);
                break;

            case 'S':
                if (argv[3] != NULL)
                    days = strtod(argv[3], NULL);
                habit_print_dates(home_path, argv[2], days);
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

        char *plname = alloccat(argv[2], ".playlist");
        char *plpath = alloccat(home_path, plname);

        switch (argv[1][2]) {

            case 'c':
                if (argv[2] == NULL)
                    break;
                playlist_create(plpath);
                break;

            case 's':
                if (argv[3])
                    days = strtod(argv[3], NULL);
                playlist_print(home_path, plname, days);
                break;

            case 'a':
                if (argv[3] == NULL)
                    break;
                playlist_add(home_path, argv[3], plname);
                playlist_print(home_path, plname, days);
                break;

            case 'r':
                if (argv[3] == NULL)
                    break;
                playlist_remove(home_path, argv[3], plname);
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
