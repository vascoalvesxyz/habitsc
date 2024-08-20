#include "config.h"
#include "habits.h"
#include <stdio.h>

static const unsigned int BUFSIZE_FGETS = 31;

time_t date_midnight(time_t time) {
        struct tm *lnow = localtime(&time);
        lnow->tm_min = 0;
        lnow->tm_sec = 0;
        lnow->tm_hour = 0;
        return mktime(lnow);
}

/*void habit_mark(char* habit_path) {*/
/*    FILE* file_to_read = fopen(habit_path, "r");*/
/*    if( file_to_read == NULL ) {*/
/*        puts("Habit doesn't exist!");*/
/*    } else {*/
/*        char buf[31];*/
/*        fgets(buf, 31 , file_to_read);*/
/*        time_t line1 =  */
/*        fclose(file_to_read);*/
/*        if (date_midnight(time(NULL)) <= line1 ) {*/
/*            puts("Today already marked!");*/
/*        } else {*/
/*            sprintf(buf, "%ld", date_midnight(NULL) );*/
/*            file_prepend(habit_path, buf);*/
/*        }*/
/*    }*/
/*}*/

void habit_mark(char* habit_path) {
    FILE* file_to_read = fopen(habit_path, "r");
    unsigned long line1 = FALSE;

    if (file_to_read != NULL) {
        char buf[32];
        if (fgets(buf, sizeof(buf), file_to_read) != NULL) {

            char* endptr;
            line1 = strtoul(buf, &endptr, 10);

            if (endptr == buf || (*endptr != '\n' && *endptr != '\0')) {
                line1 = FALSE; 
            }
        }
        fclose(file_to_read);
    }

    time_t current_midnight = date_midnight(time(NULL));

    if (current_midnight <= (time_t)line1) {
        puts("Today already marked!");
    } else {
        char buf[32];
        sprintf(buf, "%ld", (long)current_midnight);
        file_prepend(habit_path, buf);
    }

}



int habit_create(char* habit_path) {
        int res = file_touch(habit_path);
        switch (res) {
            case -1:
                puts("Habit already exists!");
                break;
            case TRUE:
                puts("Habit successfully created!");
                break;
        }
        return res;
}

int habit_delete(char *habit_path) {
    FILE* fhabit = fopen(habit_path, "r");
    if (fhabit == NULL) {
        return -1;
    } else {
        char input;
        printf("\tAre you sure you want to delete this habit forever? (y/N)\n\t(you could archive instead) ");
        scanf("%c", &input);
        if (input != 'y' ) {
            fclose(fhabit);
            return 0;
        } else {
            remove(habit_path);
        }
    }
    fclose(fhabit);
    return TRUE;
}


void habit_print(char* home, char* habit_name, unsigned int total_days) {

    if (ASCII_MODE >= 1) {
        LEFT_ELBOW = "|";
        MID_ELBOW = "|";
        RIGHT_ELBOW = "|";
        HBAR = "-";
        VBAR_THIN = "|";
        VBAR_THICK = "|";
    }

    FILE * file_to_Read;
    char* habitpath = alloccat(home, habit_name);
    file_to_Read = fopen(habitpath, "r");

    if (file_to_Read == NULL){
        printf("Habit %s does not exist! (%s)\n", habit_name, habitpath);
    } else {
        unsigned int c = 0;
        time_t right;
        time_t left = date_midnight(time(NULL))+(24*60*60);

        if (SEPERATE_HABITS >= 1) draw_hseperators(total_days);

        printf("%s%*s %s ", VBAR_THICK, 16, habit_name, VBAR_THIN);
        char buffer[31]; 

        /* Print marked dates using left and right pointers */
        while (NULL != fgets(buffer, BUFSIZE_FGETS, file_to_Read)) {
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
        puts(VBAR_THICK);
        fclose(file_to_Read);
    }

    free(habitpath);
    if (total_days > 50000) {
        printf("The number of days ago");
        red();
        printf(" EXCEEDS 50000 DAYS ");
        reset();
        puts("(aprox. 136 years)\n Hello immortal vampires and time travelers!");
    }
}

