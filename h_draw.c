#include "config.h"
#include "habits.h"

static const unsigned int NAME_PADDING = 16;

void red() { printf("\033[1;31m"); }
void blue() { printf("\033[0;34m"); }
void reset() { printf("\033[0m"); }

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
