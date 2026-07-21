/*
 * login.c — Authentication with masked password input
 */
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "login.h"
#include "ui.h"

#define ADMIN_USER "admin"
#define ADMIN_PASS "admin123"

static void maskedInput(char *buf, int maxlen) {
    struct termios old, noecho;
    tcgetattr(STDIN_FILENO, &old);
    noecho = old;
    noecho.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &noecho);

    int i = 0;
    int c;
    while (i < maxlen - 1) {
        c = getchar();
        if (c == '\n' || c == EOF) break;
        if (c == 127 || c == '\b') {   /* backspace */
            if (i > 0) {
                i--;
                printf("\b \b");
                fflush(stdout);
            }
        } else {
            buf[i++] = (char)c;
            printf("*");
            fflush(stdout);
        }
    }
    buf[i] = '\0';
    printf("\n");

    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}

int login(char *out_user, int out_size) {
    char username[64];
    char password[64];

    clearScreen();

    /* Banner */
    printf("\n");
    printHLine(BOX_TL, BOX_H, BOX_TR);
    printBoxBlank();
    printBoxTitle("STUDENT MANAGEMENT SYSTEM  v2.0");
    printBoxBlank();
    printBoxTitle("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    printBoxBlank();
    printBoxTitle("Please sign in to continue");
    printBoxBlank();
    printHLine(BOX_BL, BOX_H, BOX_BR);

    printf("\n");
    readString("Username : ", username, sizeof(username));

    printf("  Password : ");
    fflush(stdout);
    maskedInput(password, sizeof(password));

    if (strcmp(username, ADMIN_USER) == 0 &&
        strcmp(password, ADMIN_PASS) == 0) {
        strncpy(out_user, username, out_size - 1);
        out_user[out_size - 1] = '\0';
        return 1;
    }
    return 0;
}
