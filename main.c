#include "config.h"
#include "habits.h"

int
main(int argc, char *argv[]) {

    int days = DEFAULT_DAYS;
    char* home_path = alloccat(getenv("HOME"),  "/.local/share/habits/");

    if (argc <= 2) {
        if (argv[1]) days = strtod(argv[1], NULL) ;
        playlist_print(home_path, ALL, days);
    }

    else if (argv[1][1] == 'h') {

        char* habit_path = alloccat(home_path, argv[2]);

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
            break;
        case 'd':
            if (habit_delete(habit_path) == TRUE) {
                playlist_remove(home_path, argv[2], ALL);
            }
            playlist_print(home_path, ALL, days);
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
            if (argv[1] != NULL) days = strtod(argv[1], NULL);
            playlist_print(home_path, ALL, days);
            break;

        free(habit_path);
    } /* end of switch */
    free(habit_path);
    } /* end of if */
    else if (argv[1][1] == 'p' && argv[2]) {
        int days = DEFAULT_DAYS;

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
    free(home_path);
    return 0;
}
