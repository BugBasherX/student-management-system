#ifndef STUDENT_H
#define STUDENT_H

#define MAX_NAME_LEN  64
#define MAX_COURSE_LEN 64
#define DATA_FILE "students.dat"

typedef struct {
    int  id;
    char name[MAX_NAME_LEN];
    int  age;
    char course[MAX_COURSE_LEN];
    float gpa;
} Student;

void addStudent();
void viewStudents();
void searchStudent();
void updateStudent();
void deleteStudent();

#endif
