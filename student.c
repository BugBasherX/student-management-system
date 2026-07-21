/*
=========================================
 Student Management System
 File: student.c
=========================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

/* ---- helpers ---- */

static int nextId() {
    FILE *f = fopen(DATA_FILE, "rb");
    if (!f) return 1;

    int maxId = 0;
    Student s;
    while (fread(&s, sizeof(Student), 1, f) == 1) {
        if (s.id > maxId) maxId = s.id;
    }
    fclose(f);
    return maxId + 1;
}

static int countStudents() {
    FILE *f = fopen(DATA_FILE, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    int count = (int)(ftell(f) / sizeof(Student));
    fclose(f);
    return count;
}

/* ---- public functions ---- */

void addStudent() {
    Student s;
    s.id = nextId();

    printf("\n--- ADD STUDENT ---\n");
    printf("Name   : ");
    scanf(" %63[^\n]", s.name);
    printf("Age    : ");
    scanf("%d", &s.age);
    printf("Course : ");
    scanf(" %63[^\n]", s.course);
    printf("GPA    : ");
    scanf("%f", &s.gpa);

    FILE *f = fopen(DATA_FILE, "ab");
    if (!f) {
        printf("Error: could not open data file.\n");
        return;
    }
    fwrite(&s, sizeof(Student), 1, f);
    fclose(f);

    printf("\nStudent added successfully! (ID: %d)\n", s.id);
}

void viewStudents() {
    FILE *f = fopen(DATA_FILE, "rb");
    if (!f || countStudents() == 0) {
        printf("\nNo students found.\n");
        if (f) fclose(f);
        return;
    }

    printf("\n--- STUDENT LIST ---\n");
    printf("%-6s %-20s %-5s %-20s %s\n",
           "ID", "Name", "Age", "Course", "GPA");
    printf("--------------------------------------------------------------\n");

    Student s;
    while (fread(&s, sizeof(Student), 1, f) == 1) {
        printf("%-6d %-20s %-5d %-20s %.2f\n",
               s.id, s.name, s.age, s.course, s.gpa);
    }
    fclose(f);
}

void searchStudent() {
    if (countStudents() == 0) {
        printf("\nNo students found.\n");
        return;
    }

    int id;
    printf("\n--- SEARCH STUDENT ---\n");
    printf("Enter Student ID: ");
    scanf("%d", &id);

    FILE *f = fopen(DATA_FILE, "rb");
    if (!f) { printf("Error: could not open data file.\n"); return; }

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, f) == 1) {
        if (s.id == id) {
            printf("\n  ID     : %d\n", s.id);
            printf("  Name   : %s\n", s.name);
            printf("  Age    : %d\n", s.age);
            printf("  Course : %s\n", s.course);
            printf("  GPA    : %.2f\n", s.gpa);
            found = 1;
            break;
        }
    }
    fclose(f);

    if (!found) printf("\nStudent with ID %d not found.\n", id);
}

void updateStudent() {
    if (countStudents() == 0) {
        printf("\nNo students found.\n");
        return;
    }

    int id;
    printf("\n--- UPDATE STUDENT ---\n");
    printf("Enter Student ID to update: ");
    scanf("%d", &id);

    /* Load all records */
    FILE *f = fopen(DATA_FILE, "rb");
    if (!f) { printf("Error: could not open data file.\n"); return; }

    int count = countStudents();
    Student *buf = malloc(count * sizeof(Student));
    if (!buf) { fclose(f); printf("Memory error.\n"); return; }

    int n = 0;
    Student s;
    while (fread(&s, sizeof(Student), 1, f) == 1) buf[n++] = s;
    fclose(f);

    int found = 0;
    for (int i = 0; i < n; i++) {
        if (buf[i].id == id) {
            printf("Enter new Name   [%s]: ", buf[i].name);
            scanf(" %63[^\n]", buf[i].name);
            printf("Enter new Age    [%d]: ", buf[i].age);
            scanf("%d", &buf[i].age);
            printf("Enter new Course [%s]: ", buf[i].course);
            scanf(" %63[^\n]", buf[i].course);
            printf("Enter new GPA    [%.2f]: ", buf[i].gpa);
            scanf("%f", &buf[i].gpa);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("\nStudent with ID %d not found.\n", id);
    } else {
        f = fopen(DATA_FILE, "wb");
        if (f) {
            fwrite(buf, sizeof(Student), n, f);
            fclose(f);
            printf("\nStudent updated successfully!\n");
        } else {
            printf("Error: could not save changes.\n");
        }
    }
    free(buf);
}

void deleteStudent() {
    if (countStudents() == 0) {
        printf("\nNo students found.\n");
        return;
    }

    int id;
    printf("\n--- DELETE STUDENT ---\n");
    printf("Enter Student ID to delete: ");
    scanf("%d", &id);

    FILE *f = fopen(DATA_FILE, "rb");
    if (!f) { printf("Error: could not open data file.\n"); return; }

    int count = countStudents();
    Student *buf = malloc(count * sizeof(Student));
    if (!buf) { fclose(f); printf("Memory error.\n"); return; }

    int n = 0, found = 0;
    Student s;
    while (fread(&s, sizeof(Student), 1, f) == 1) {
        if (s.id == id) { found = 1; continue; }
        buf[n++] = s;
    }
    fclose(f);

    if (!found) {
        printf("\nStudent with ID %d not found.\n", id);
    } else {
        f = fopen(DATA_FILE, "wb");
        if (f) {
            fwrite(buf, sizeof(Student), n, f);
            fclose(f);
            printf("\nStudent deleted successfully!\n");
        } else {
            printf("Error: could not save changes.\n");
        }
    }
    free(buf);
}
