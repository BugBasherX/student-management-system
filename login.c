/*
 * login.c — Authentication with masked password + runtime password change
 *
 * Password storage: admin.cfg (plain text, one line).
 * Falls back to compiled-in default if the file is absent.
 */
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "login.h"
#include "ui.h"

#define ADMIN_USER     "admin"
#define DEFAULT_PASS   "admin123"
#define PASS_FILE      "admin.cfg"

/* ── Password file helpers ───────────────────────────────── */

static void loadPassword(char *buf, int maxlen) {
    FILE *f = fopen(PASS_FILE, "r");
    if (!f) {
        strncpy(buf, DEFAULT_PASS, maxlen - 1);
        buf[maxlen - 1] = '\0';
        return;
    }
    if (fgets(buf, maxlen, f)) {
        buf[strcspn(buf, "\r\n")] = '\0';
    } else {
        strncpy(buf, DEFAULT_PASS, maxlen - 1);
        buf[maxlen - 1] = '\0';
    }
    fclose(f);
}

static int savePassword(const char *pass) {
    FILE *f = fopen(PASS_FILE, "w");
    if (!f) return 0;
    fprintf(f, "%s\n", pass);
    fclose(f);
    return 1;
}

/* ── Masked input ────────────────────────────────────────── */

static void maskedInput(char *buf, int maxlen) {
    struct termios old, noecho;
    tcgetattr(STDIN_FILENO, &old);
    noecho = old;
    noecho.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &noecho);

    int i = 0, c;
    while (i < maxlen - 1) {
        c = getchar();
        if (c == '\n' || c == EOF) break;
        if (c == 127 || c == '\b') {
            if (i > 0) { i--; printf("\b \b"); fflush(stdout); }
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

/* ── Public: login ───────────────────────────────────────── */

int login(char *out_user, int out_size) {
    char stored[128];
    loadPassword(stored, sizeof(stored));

    char username[64];
    char password[128];

    clearScreen();
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
        strcmp(password, stored) == 0) {
        strncpy(out_user, username, out_size - 1);
        out_user[out_size - 1] = '\0';
        return 1;
    }
    return 0;
}

/* ── Public: changePassword ──────────────────────────────── */

void changePassword(void) {
    char stored[128];
    loadPassword(stored, sizeof(stored));

    clearScreen();
    printf("\n");
    printHLine(BOX_TL, BOX_H, BOX_TR);
    printBoxTitle("CHANGE PASSWORD");
    printHLine(BOX_ML, BOX_H, BOX_MR);
    printBoxBlank();
    printHLine(BOX_BL, BOX_H, BOX_BR);
    printf("\n");

    /* Verify current password */
    char current[128];
    printf("  Current password : ");
    fflush(stdout);
    maskedInput(current, sizeof(current));

    if (strcmp(current, stored) != 0) {
        printError("Incorrect current password.");
        pauseScreen();
        return;
    }

    /* Get new password */
    char newpass[128], confirm[128];

    printf("  New password     : ");
    fflush(stdout);
    maskedInput(newpass, sizeof(newpass));

    if (strlen(newpass) < 4) {
        printError("Password must be at least 4 characters.");
        pauseScreen();
        return;
    }

    printf("  Confirm password : ");
    fflush(stdout);
    maskedInput(confirm, sizeof(confirm));

    if (strcmp(newpass, confirm) != 0) {
        printError("Passwords do not match.");
        pauseScreen();
        return;
    }

    if (savePassword(newpass)) {
        printSuccess("Password changed successfully.");
        printInfo("New password is active immediately.");
    } else {
        printError("Failed to save password file.");
    }
    pauseScreen();
}
