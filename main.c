#include <stdio.h>
#include <stdlib.h>
#include "login.h"
#include "student.h"

void menu() {
    printf("\n=========================================\n");
    printf("      STUDENT MANAGEMENT SYSTEM\n");
    printf("=========================================\n");
    printf("1. Add Student\n");
    printf("2. View Students\n");
    printf("3. Search Student\n");
    printf("4. Update Student\n");
    printf("5. Delete Student\n");
    printf("6. Student Statistics\n");
    printf("7. Exit\n");
    printf("=========================================\n");
    printf("Enter your choice: ");
}

int main() {
    int choice;

    printf("=========================================\n");
    printf("      WELCOME TO STUDENT SYSTEM\n");
    printf("=========================================\n");

    if (!login()) {
        printf("\nAccess Denied!\n");
        return 0;
    }

    while (1) {
        menu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                addStudent();
                break;

            case 2:
                viewStudents();
                break;

            case 3:
                searchStudent();
                break;

            case 4:
                updateStudent();
                break;

            case 5:
                deleteStudent();
                break;

            case 6:
                studentStatistics();
                break;

            case 7:
                printf("\nThank you for using the Student Management System.\n");
                exit(0);

            default:
                printf("\nInvalid choice! Please try again.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        system("cls || clear");
    }

    return 0;
}
