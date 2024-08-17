#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "file.h"
#include "config.h"

#define ALL "all.playlist"

#define TRUE 1
#define FALSE 0

int
main(int argc, char *argv[]) {
    char* path = alloccat(getenv("HOME"),  "/.local/share/habits/");
    if (argc <= 2) {
        int days = DEFAULT_DAYS;
        if (argv[1] != NULL) days = strtod(argv[1], NULL);
        draw_tbar(days, ALL);
        print_habitsn_from_playlist_file(path, ALL, days);
        draw_bbar(days);
    }
    /* status ✅✅ */
    else if (strcmp(argv[1], "status") == 0 && (argc == 4 || argc == 3)) {
        int days = DEFAULT_DAYS;
        if (argv[3] != NULL) days = strtod(argv[3], NULL);
        draw_tbar(days, "STATUS");
        print_habitn_from_file(path, argv[2], days);
        draw_bbar(days);
    }

    /* add ✅✅ */
    else if (strcmp(argv[1], "add") == 0 && argc == 3) {
        char *habit_file_path = alloccat(path, argv[2]);
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
        char *habit_file_path = alloccat(path, argv[2]);
        FILE* fhabit = fopen(habit_file_path, "r");
        if (fhabit == NULL) {
            printf("Habit does not exist!\n");
        } else {
            char input;
            printf("\tAre you sure you want to delete this habit forever? (y/N)\n\t(you could archive instead) ");
            scanf("%c", &input);
            if (input == 'y' ) {
                remove_from_playlist(path, argv[2], ALL);
                remove(habit_file_path);
            }
            fclose(fhabit);
        }
        free(habit_file_path);
        print_habitsn_from_playlist_file(path, ALL, 7);
    }

    /* Playlist stuff */
    else if (strcmp(argv[1], "playlist") == 0) {
        if (strlen(argv[2]) == 2 && argv[2][0] == '-' && (argc == 4 || argc == 5)) {
            int days = DEFAULT_DAYS;
            char *fp = NULL;
            FILE* fplaylist = NULL;
            char* plpath = alloccat(argv[3], ".playlist");
            switch (argv[2][1]) {
                case 'c':
                    fp = alloccat(path, plpath);
                    fplaylist = fopen(fp, "r");
                    if( fplaylist != NULL ) {
                        printf("Playlist already exists!\n");
                    }
                    else {
                        fplaylist = fopen(fp, "w"); 
                        printf("Playlist %s successfully created!\n", argv[3]);
                    }
                    break;

                case 's':
                    if (argv[4]) days = strtod(argv[4], NULL);
                    if (argv[3]) print_habitsn_from_playlist_file(path, plpath, days);
                    else puts("Provide name of playlist to show!");
                    break;

                case 'a':
                    if (strcmp(plpath, "all.playlist")) {
                        fp = alloccat(path, argv[4]);
                        fplaylist = fopen(fp, "r");
                        if (fplaylist == NULL) {
                            printf("Habit %s doesn't exist!\n", argv[4]);
                        }
                        else {
                            fclose(fplaylist); // close habit file before opening playlist
                            free(fp); // free memory of playlist path
                            fp = alloccat(path, plpath);
                            fplaylist = fopen(fp, "r");

                            if (fplaylist == NULL) {
                                puts("Playlist does not exist.");
                            }
                            else {
                                if (search_file(fplaylist, argv[4]) == FALSE) {
                                    printf("Adding %s to %s\n", argv[4], argv[3]);
                                    add_to_playlist(path, argv[4], plpath);
                                } 
                                else puts("Habit already in this playlist!");
                                //fclose(fplaylist); // playlist file
                            }
                        }

                    } else puts("No");
                    break;

                case 'd':
                    fp = alloccat(path, plpath);
                    fplaylist = fopen(fp, "r");
                    if (fplaylist) {
                        if (search_file(fplaylist, argv[4]) == TRUE)
                            remove_from_playlist(path, argv[4], plpath);
                        else puts("Habit not in this playlist!");
                    }
                    else puts("Playlist does not exist!");
                    break;
                default:
                    puts("Not a valid flag!");
            }
            if (plpath) free(plpath);
            if (fplaylist != NULL) fclose(fplaylist);
            if (fp != NULL) free(fp);
        } else {
            printf("Wrong number of arguments.\n");
        }
    }

    /* default */
    else {
        printf("Not a valid command!\n");
    }
    free(path);
    return 0;
}
