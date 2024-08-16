#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "file.h"

#define ALL "all.playlist"

#define TRUE 1
#define FALSE 0


int diffdate(time_t t1, time_t t2) {
    time_t diff = difftime(t1, t2);
    struct tm* l = localtime(&diff);
    return abs(l->tm_yday + ((int) 365.25*(l->tm_year-70))) -1;    
}

int
main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("[status|add|mark] name\n");
        return 0;
    }

    char* path = alloc_real_path(getenv("HOME"),  "/.local/share/habits/");

    /* status ✅✅ working and valgrind checked */
    if (strcmp(argv[1], "status") == 0 && (argc == 4 || argc == 3)) {
        int days;
        if (argv[3] == NULL) days = 7;
        else days = strtod(argv[3], NULL);
        printf("Displaying last %d days\n", days);
        print_habitn_from_file(path, argv[2], days);
    }

    /* add ✅✅ working and valgrind checked */
    else if (strcmp(argv[1], "add") == 0 && argc == 3) {
        char *habit_file_path = alloc_real_path(path, argv[2]);
        FILE* fread = fopen(habit_file_path, "r");
        if( fread != NULL ) {
            printf("Habit already exists!\n");
            fclose(fread);
        } else {
            FILE* fcreate;
            fcreate = fopen(habit_file_path, "w"); // fails if no directory
            if (fcreate == NULL) {
                perror("ERROR: Please create the ./local/share/habits directory!\n") ;
            } else {
                add_to_playlist(path, argv[2], ALL);
                fclose(fcreate);
            }
            print_habitsn_from_playlist_file(path, ALL, 7);
        }
        free(habit_file_path);
    }

    /* mark today ✅✅ */
    else if (strcmp(argv[1], "mark") == 0 && argc == 3) {
        mark_in_file(path, argv[2]);
        print_habitsn_from_playlist_file(path, ALL, 7);
    }

    /* del ✅✅ */
    else if (strcmp(argv[1], "del") == 0 && argc == 3) {
        remove_from_playlist(path, argv[2], ALL);
        print_habitsn_from_playlist_file(path, ALL, 7);
    }

    /* default */
    else {
        printf("Not a valid command!\n");
    }

    free(path);

    return 0;
}
