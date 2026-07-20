/*
=========================================
 Student Management System
 File: main.c
=========================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "login.h"
#include "student.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void displayMenu() {
    printf("\n====================================\n");
    printf("     STUDENT MANAGEMENT SYSTEM\n");
    printf("====================================\n");
    printf("1. Login\n");
    printf("2. Add Student\n");
    printf("3. View Students\n");
    printf("4. Search Student\n");
    printf("5. Update Student\n");
    printf("6. Delete Student\n");
    printf("7. Exit\n");
    printf("====================================\n");
    printf("Enter Choice: ");
}

int main() {
    int choice;
    int loggedIn = 0;

    while (1) {
        clearScreen();
        displayMenu();

        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input!\n");

            while (getchar() != '\n');

            pauseScreen();
            continue;
        }

        switch (choice) {

            case 1:
                loggedIn = login();

                if (loggedIn)
                    printf("\nLogin Successful!\n");
                else
                    printf("\nInvalid Username or Password!\n");

                pauseScreen();
                break;

            case 2:

                if (!loggedIn) {
                    printf("\nPlease login first!\n");
                } else {
                    addStudent();
                }

                pauseScreen();
                break;

            case 3:

                if (!loggedIn) {
                    printf("\nPlease login first!\n");
                } else {
                    viewStudents();
                }

                pauseScreen();
                break;

            case 4:

                if (!loggedIn) {
                    printf("\nPlease login first!\n");
                } else {
                    searchStudent();
                }

                pauseScreen();
                break;

            case 5:

                if (!loggedIn) {
                    printf("\nPlease login first!\n");
                } else {
                    updateStudent();
                }

                pauseScreen();
                break;

            case 6:

                if (!loggedIn) {
                    printf("\nPlease login first!\n");
                } else {
                    deleteStudent();
                }

                pauseScreen();
                break;

            case 7:

                printf("\nThank you for using Student Management System.\n");
                exit(0);

            default:

                printf("\nInvalid Choice!\n");
                pauseScreen();
        }
    }

    return 0;
}
