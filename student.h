#ifndef STUDENT_H
#define STUDENT_H

#define DATA_FILE "students.dat"

#define MAX_NAME   64
#define MAX_COURSE 64
#define MAX_EMAIL  64
#define MAX_PHONE  20

typedef struct {
    int   id;
    char  name  [MAX_NAME];
    int   age;
    char  course[MAX_COURSE];
    float gpa;
    char  email [MAX_EMAIL];
    char  phone [MAX_PHONE];
    int   year;     /* enrollment year */
} Student;

/* Sort keys */
typedef enum {
    SORT_ID = 0,
    SORT_NAME,
    SORT_GPA_DESC,
    SORT_GPA_ASC,
    SORT_AGE,
    SORT_COURSE,
    SORT_YEAR
} SortKey;

/* CRUD */
void addStudent   (void);
void viewStudents (SortKey key);
void searchStudent(void);
void updateStudent(void);
void deleteStudent(void);
void sortMenu     (void);
void statistics   (void);
void exportCSV    (void);
void topStudents  (void);   /* top-N leaderboard by GPA      */
void courseRoster (void);   /* all students in a given course */

/* Utilities */
int   countStudents(void);
float avgGPA       (void);

#endif
