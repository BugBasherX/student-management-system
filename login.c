/*
=========================================
 Student Management System
 File: login.c
=========================================
*/

#include <stdio.h>
#include <string.h>
#include "login.h"

/* Default credentials — change as needed */
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "admin123"

int login() {
    char username[32];
    char password[32];

    printf("\n--- LOGIN ---\n");
    printf("Username: ");
    scanf("%31s", username);
    printf("Password: ");
    scanf("%31s", password);

    if (strcmp(username, ADMIN_USERNAME) == 0 &&
        strcmp(password, ADMIN_PASSWORD) == 0) {
        return 1;
    }
    return 0;
}
