/*
 * main.c — Entry point and menu flow
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ui.h"
#include "login.h"
#include "student.h"

#define MAX_ATTEMPTS 3

/* ── Status bar ──────────────────────────────────────────── */

static void printStatusBar(const char *user) {
    int   total = countStudents();
    float avg   = avgGPA();

    char left[64], right[64];
    snprintf(left,  sizeof(left),  "  Signed in as: " C_BWHITE "%s" C_RESET, user);
    snprintf(right, sizeof(right),
             C_DIM "Students: " C_RESET C_BWHITE "%d" C_RESET
             C_DIM "   Avg GPA: " C_RESET "%s%.2f" C_RESET "  ",
             total, gpaColor(avg), avg);

    /* Print the status bar line */
    printf(BOX_V "%s", left);
    /* Fill middle gap */
    int llen = 18 + (int)strlen(user);
    int rlen = 22 + (total > 99 ? 3 : total > 9 ? 2 : 1);
    int gap  = BOX_WIDTH - 2 - llen - rlen;
    if (gap < 1) gap = 1;
    printf("%*s", gap, "");
    printf("%s" BOX_V "\n", right);
}

/* ── Main menu ───────────────────────────────────────────── */

static void displayMainMenu(const char *user) {
    clearScreen();
    printf("\n");
    printHLine(BOX_TL, BOX_H, BOX_TR);
    printBoxTitle("STUDENT MANAGEMENT SYSTEM  v2.0");
    printHLine(BOX_ML, BOX_H, BOX_MR);
    printStatusBar(user);
    printHLine(BOX_ML, BOX_H, BOX_MR);
    printBoxBlank();

    printf(BOX_V "  " C_CYAN "[1]" C_RESET " Add Student          "
                  C_CYAN "[5]" C_RESET " Delete Student"
           "%*s" BOX_V "\n", BOX_WIDTH - 42, "");

    printf(BOX_V "  " C_CYAN "[2]" C_RESET " View All Students    "
                  C_CYAN "[6]" C_RESET " Sort & View"
           "%*s" BOX_V "\n", BOX_WIDTH - 39, "");

    printf(BOX_V "  " C_CYAN "[3]" C_RESET " Search Student       "
                  C_CYAN "[7]" C_RESET " Statistics Report"
           "%*s" BOX_V "\n", BOX_WIDTH - 45, "");

    printf(BOX_V "  " C_CYAN "[4]" C_RESET " Update Student       "
                  C_CYAN "[8]" C_RESET " Export to CSV"
           "%*s" BOX_V "\n", BOX_WIDTH - 41, "");

    printBoxBlank();
    printHLine(BOX_ML, BOX_H, BOX_MR);

    printf(BOX_V "  " C_DIM "[0] Logout" C_RESET
           "  " C_DIM "[Q] Quit" C_RESET
           "%*s" BOX_V "\n", BOX_WIDTH - 23, "");

    printHLine(BOX_BL, BOX_H, BOX_BR);
    printf("\n");
}

/* ── Application loop ────────────────────────────────────── */

int main(void) {
    char user[64];

    while (1) {
        /* ── Login phase ── */
        int attempts = 0;
        int authed   = 0;

        while (attempts < MAX_ATTEMPTS) {
            authed = login(user, sizeof(user));
            if (authed) {
                printSuccess("Login successful! Welcome back.");
                break;
            }
            attempts++;
            int left = MAX_ATTEMPTS - attempts;
            if (left > 0)
                printf("\n  " C_BRED "Incorrect credentials. %d attempt%s remaining.\n" C_RESET,
                       left, left == 1 ? "" : "s");
            else
                printf("\n  " C_BRED "Account locked — too many failed attempts.\n" C_RESET);
        }

        if (!authed) {
            printf("\n  Exiting for security. Goodbye.\n\n");
            return 1;
        }

        /* ── Main menu loop ── */
        while (1) {
            displayMainMenu(user);

            printf("  " C_BOLD "Choice: " C_RESET);
            fflush(stdout);

            char line[16];
            if (!fgets(line, sizeof(line), stdin)) continue;
            char ch = tolower((unsigned char)line[0]);

            if (ch == 'q') {
                clearScreen();
                printf("\n  " C_DIM "Thank you for using SMS v2.0. Goodbye.\n\n" C_RESET);
                return 0;
            }

            if (ch == '0') {
                printInfo("Logged out.");
                break;   /* back to login */
            }

            int choice = ch - '0';
            printf("\n");

            switch (choice) {
                case 1: addStudent();           break;
                case 2: viewStudents(SORT_ID);  break;
                case 3: searchStudent();        break;
                case 4: updateStudent(); pauseScreen(); break;
                case 5: deleteStudent(); pauseScreen(); break;
                case 6: sortMenu();             break;
                case 7: statistics();           break;
                case 8: exportCSV();            break;
                default:
                    printWarning("Invalid choice — enter 1-8, 0, or Q.");
                    pauseScreen();
            }
        }
    }
    return 0;
}
