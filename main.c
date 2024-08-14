#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

typedef struct Node {
    struct Node* next;
    struct Node* prev;
    time_t rawtime;
} Node;


typedef struct Root {
    struct Node* first; struct Node* last;
    struct Root* next;
    struct Root* prev;
    char* name;
} Root;

typedef struct Habits {
    struct Root* first;
    struct Root* last;
    int number;
} Habits;

int diffdate(time_t t1, time_t t2) {
    // larger smaller
    //if ( (smaller day <= larger day && month == month && tm_year == tm_year)
    //      || ( t_mon < t_mon && tm_year == tm_year)
    //      || (tm_year < tm_year)
    //) {
    //  
    //}
    time_t diff = difftime(t1, t2);
    struct tm* l = localtime(&diff);
    return abs(l->tm_yday + ((int) 365.25*(l->tm_year-70))) -1;    
}

Habits* init_habits() {
    Habits* new = (Habits*) malloc(sizeof(struct Habits));
    new->first = NULL;
    new->last = NULL;
    new->number = 0;
    return new;
}

void init_root(Habits *habits, char* name) {

    Root* new = (Root*) malloc(sizeof(struct Root));
    new->first = NULL;
    new->last = NULL;

    new->name = (char*) malloc(sizeof(char)*(strlen(name)));
    new->name = strdup(name);

    /* if this is the first habit */
    if (habits->first == NULL) {
        new->prev = NULL;
        new->next = NULL;
        habits->first = new;
        habits->last  = new;
    /* else add it to the end*/
    } else {
        new->prev = habits->last;
        new->prev->next = new;
        new->next = NULL;
        habits->last  = new;
    }

    habits->number += 1;
}

void print_habits(Habits* habits) {
    if (habits->first == NULL) {
        printf("No habits created yet!");
        return;
    }

    Root* ptr = habits->first;

    /* iterate habits */
    while (ptr != NULL) {
        printf("%10s | ", ptr->name);
        Node* left = ptr->last;
        /* empty list */
        if (left == NULL) {
            printf("o o o o o o o \n");
            ptr = ptr->next;
            continue;
        }
        Node* right = left->prev;
        int c = 0;

        time_t now = time(NULL);
        int dif = diffdate(left->rawtime , now);

        if (dif > 7) {
            printf("o o o o o o o \n");
            ptr = ptr->next;
            continue;
        }

        /* if left is not today, fill in the spaces before */
        if (dif > 0) {
            for (int i = 0; i < dif; i++) {
                if (c>=7) break;
                printf("o ");
                c += 1;
            }
        }
        printf("x ");
        c += 1;

        while (right != NULL) {
            /* print empty days */
            int dif = diffdate(left->rawtime , right->rawtime);
            if (dif >= 0) {
                for (int i = 0; i < dif; i++) {
                    if (c>6) break;
                    printf("o ");
                    c += 1;
                }
            }

            printf("x ");
            c += 1;
            if (c>6) break;
            left = right;
            right = left->prev;
        }
        if (c < 6) {
            for (int i = c; i <= 6; i++) {
                    printf("o ");
            }
        }
        printf("\n");
        ptr = ptr->next;
    }
}

void insert_date(Root* h, int day, int month, int year) {
    Node* ptr;

    struct tm given = {0};
    given.tm_year = year-1900;
    given.tm_mon = month -1;
    given.tm_mday = day;
    given.tm_hour = 0;
    given.tm_min = 0;
    given.tm_sec = 0;
    given.tm_isdst = -1;

    time_t given_time = mktime(&given);
    /* case -1: in the future */
    if (given_time >= time(NULL)) {
        printf("No time travel!");
        return;
    }

    Node* new = (Node*) malloc(sizeof(struct Node));
    new->rawtime = given_time;

    /* case 0: the linked list is empty */
    if (h->first == NULL) {
        h->first = new;
        h->last = new;
        new->prev = NULL;
        new->next = NULL;
        return;
    }
    /* case 1: this is the oldest date */
    else if (h->first->rawtime > given_time) {
        new->prev = NULL;
        new->next = h->first;
        h->first->prev = new;
        h->first = new;
        return;
    }
    /* case 2: this is the most recent date */
    else if (h->last->rawtime < given_time) {
        new->next = NULL;
        new->prev = h->last ;
        h->last->next = new;
        h->last = new;
        return;
    }
    /* else: travel back to find where to place new node */
    else {
        for (ptr = h->last; ptr->prev != NULL; ptr = ptr->prev) {
            /* check if already marked */
            if (ptr->rawtime == given_time || ptr->prev->rawtime == given_time) {
                printf("Already marked.\n");
                free(new);
                return;
            }
            /* if found spot */
            if (ptr->prev->rawtime < given_time && ptr->rawtime > given_time) {
                new->prev = ptr->prev;
                new->next = ptr;
                new->prev->next = new;
                new->next->prev = new;
                printf("Found spot.");
                return;
            }
        }
        printf("No spot found dafuq???");
    }
    return;
};

void destroy_list(Habits* h, Root* r) {  
    if (h == NULL || r == NULL)
        return;

    // case 0: this was the first habit
    if (h->first == r) {
        h->first = h->first->next;
        h->first->prev = NULL;
    }
    // case 1: this was the last habit
    else if (h->last == r) {
        h->last = h->last->prev;
        h->last->next = NULL;
    }
    // case 2: rout the prev to next and vice-versa 
    else {
        Root* temp = r->prev;
        temp->next = r->next;
        temp->next->prev = temp;
    }

    Node* ptr = r->first;
    Node* temp;


    while (ptr != NULL) {
        temp = ptr->next;
        free(ptr);
        ptr = temp;
    }

    free(r->name);
    free(r);
}

void destroy_everything(Habits* h) {
    Root* r = h->first;
    Root* temp;

    while (r != NULL) {
        temp = r->next;
        //printf("CLOSE: Deleting %s\n", r->name);
        Node* ptr = r->first;
        Node* temp2;
        while (ptr != NULL) {
            temp2 = ptr->next;
            free(ptr);
            ptr = temp2;
        }
        free(r->name);
        free(r);

        r = temp;
    }

    free(h);
}

void insert_today(Root* h) {
    time_t now = time(NULL);
    struct tm *tday = localtime(&now);
    insert_date(h, tday->tm_mday, tday->tm_mon+1, tday->tm_year+1900);
}

Habits* read_from_file() {
    char* home = getenv("HOME");
    strcat(home, "/.local/habits.txt"); /* env is now permanently altered */

    FILE* fptr = fopen(home, "rt");
    Habits* h = init_habits();

    if(fptr==NULL) {
        printf("Error\n");
        fclose(fptr);
        return h;
    } else {
        char *s = 0;
        size_t buflen = 0;
        while (getline(&s, &buflen, fptr) != -1) {
            char* name;
            char* token;

            token = strtok(s, ",");
            name = strdup(token);
            
            init_root(h, name);
            Root* r = h->last; /*new root is always last*/
            time_t t; 
            struct tm *lt;
            while (token != NULL) {
                token = strtok(NULL, ",");
                t = strtoul(token, NULL, 10);
                if (t==0)  break; 

                lt = localtime(&t);
                insert_date(r, lt->tm_mday, lt->tm_mon+1, lt->tm_year+1900);
            };
        }
        free(s);
    }
    
    fclose(fptr);
    return h;
}

void save_data(Habits* h) {
    char* home = getenv("HOME");
    FILE* fptr = fopen(home, "wt");

    for (Root* r = h->first; r != NULL; r = r ->next) {
        fprintf(fptr, "%s,", r->name); 
        for (Node* d = r->first; d != NULL; d = d ->next) {
            fprintf(fptr, "%ld,", d->rawtime ); 
        }
        fprintf(fptr, "\n"); 
    }

    fclose(fptr);
    return;
}


Root* find_root(Habits* h, char* name) {
    for (Root* r = h->first; r != NULL; r = r->next) {
        if (strcmp(name, r->name) == 0) {
            return r;
        }
    }
    return NULL;
}

int
main(int argc, char *argv[]) {
    if (argc <= 1) return 1;

    Habits* h1 = read_from_file();

    if (strcmp(argv[1], "status") == 0 && argc == 2) {
        print_habits(h1);
    }
    else if (strcmp(argv[1], "add") == 0 && argc == 3) {
        if (find_root(h1, argv[2]) != NULL) {
            printf("Habit already exists!\n");
        } else {
            init_root(h1, argv[2]);
            print_habits(h1);
        }
    }
    else if (strcmp(argv[1], "mark") == 0 && argc == 3) {
        Root* found = find_root(h1, argv[2]);
        if (found != NULL)  {
            insert_today(found);
            print_habits(h1);
        } else 
            printf("Habit not found!\n");
    }
    else if (strcmp(argv[1], "del") == 0 && argc == 3) {
        Root* found = find_root(h1, argv[2]);
        if (found != NULL) {
            destroy_list(h1, found);
            print_habits(h1);
        } else {
            printf("Habit not found!\n");
        }
    }
    else if (strcmp(argv[1], "insert") == 0 && argc == 4) {
        Root* found = find_root(h1, argv[2]);
        if (found != NULL) {
            int day,month,year;
            sscanf(argv[3], "%02d/%02d/%04d", &day, &month, &year); 
            insert_date(found, day, month, year);
            print_habits(h1);
        } else {
            printf("Habit not found!\n");
        }
    }
    else {
        printf("Not a valid command!\n");
        printf("[status] | [add|mark] name\n");
    }

    save_data(h1);
    destroy_everything(h1);

    return 0;
}
