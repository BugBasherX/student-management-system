/*
 * student.c — CRUD, search, sort, statistics, export
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "student.h"
#include "ui.h"

/* ── Internal helpers ────────────────────────────────────── */

static int loadAll(Student **out) {
    FILE *f = fopen(DATA_FILE, "rb");
    if (!f) { *out = NULL; return 0; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    int n = (int)(sz / sizeof(Student));
    if (n == 0) { fclose(f); *out = NULL; return 0; }
    *out = malloc(n * sizeof(Student));
    if (!*out) { fclose(f); return 0; }
    fread(*out, sizeof(Student), n, f);
    fclose(f);
    return n;
}

static int saveAll(Student *arr, int n) {
    FILE *f = fopen(DATA_FILE, "wb");
    if (!f) return 0;
    fwrite(arr, sizeof(Student), n, f);
    fclose(f);
    return 1;
}

static int nextId(Student *arr, int n) {
    int max = 0;
    for (int i = 0; i < n; i++)
        if (arr[i].id > max) max = arr[i].id;
    return max + 1;
}

/* Case-insensitive substring search */
static int containsCI(const char *haystack, const char *needle) {
    char h[256], n[256];
    int i;
    for (i = 0; haystack[i] && i < 255; i++) h[i] = tolower((unsigned char)haystack[i]);
    h[i] = '\0';
    for (i = 0; needle[i] && i < 255; i++) n[i] = tolower((unsigned char)needle[i]);
    n[i] = '\0';
    return strstr(h, n) != NULL;
}

/* ── Sort comparators ────────────────────────────────────── */

static int cmpId     (const void *a, const void *b) { return ((Student*)a)->id   - ((Student*)b)->id;   }
static int cmpName   (const void *a, const void *b) { return strcmp(((Student*)a)->name,   ((Student*)b)->name);   }
static int cmpCourse (const void *a, const void *b) { return strcmp(((Student*)a)->course, ((Student*)b)->course); }
static int cmpAge    (const void *a, const void *b) { return ((Student*)a)->age  - ((Student*)b)->age;  }
static int cmpYear   (const void *a, const void *b) { return ((Student*)a)->year - ((Student*)b)->year; }
static int cmpGpaDesc(const void *a, const void *b) {
    float d = ((Student*)b)->gpa - ((Student*)a)->gpa;
    return (d > 0) - (d < 0);
}
static int cmpGpaAsc (const void *a, const void *b) {
    float d = ((Student*)a)->gpa - ((Student*)b)->gpa;
    return (d > 0) - (d < 0);
}

static void sortArr(Student *arr, int n, SortKey key) {
    typedef int (*Cmp)(const void*, const void*);
    Cmp cmps[] = { cmpId, cmpName, cmpGpaDesc, cmpGpaAsc, cmpAge, cmpCourse, cmpYear };
    qsort(arr, n, sizeof(Student), cmps[key]);
}

/* ── Table rendering ─────────────────────────────────────── */

#define PAGE_SIZE 10

/* Column widths (inner content only) */
#define W_ID   4
#define W_NAME 22
#define W_AGE  3
#define W_CRS  18
#define W_GPA  4
#define W_YR   4
/* Total inner chars: 1+W_ID+2+W_NAME+2+W_AGE+2+W_CRS+2+W_GPA+1+2+W_YR+1 = 76 */

static void printTableHeader(void) {
    /* Top border */
    printf(C_CYAN BOX_TL BOX_H BOX_H BOX_H BOX_H BOX_H       /* ID */
           BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H
           BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H  /* Name */
           BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H
           BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H              /* Age */
           BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H
           BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H  /* Course */
           BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H
           BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H        /* GPA+Grade */
           BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H  /* Year */
           BOX_TR C_RESET "\n");
    /* Header row */
    printf(C_CYAN BOX_V C_RESET
           C_BOLD C_BWHITE
           " %-*s  %-*s  %-*s  %-*s  %-*s  %-*s "
           C_RESET
           C_CYAN BOX_V C_RESET "\n",
           W_ID,   "ID",
           W_NAME, "Name",
           W_AGE,  "Age",
           W_CRS,  "Course",
           W_GPA+2,"GPA",
           W_YR,   "Year");
    /* Separator */
    printf(C_CYAN BOX_ML);
    for (int i = 0; i < BOX_WIDTH - 2; i++) printf(BOX_H);
    printf(BOX_MR C_RESET "\n");
}

static void printTableRow(const Student *s, int rowNum) {
    const char *col = (rowNum % 2 == 0) ? C_DIM : C_RESET;
    printf(C_CYAN BOX_V C_RESET "%s"
           " %-*d  %-*.*s  %-*d  %-*.*s  %s%*.2f%s  %-*d "
           C_RESET C_CYAN BOX_V C_RESET "\n",
           col,
           W_ID,   s->id,
           W_NAME, W_NAME, s->name,
           W_AGE,  s->age,
           W_CRS,  W_CRS,  s->course,
           gpaColor(s->gpa), W_GPA + 2, s->gpa, C_RESET,
           W_YR,   s->year);
}

static void printTableFooter(void) {
    printf(C_CYAN BOX_BL);
    for (int i = 0; i < BOX_WIDTH - 2; i++) printf(BOX_H);
    printf(BOX_BR C_RESET "\n");
}

/* Paginated display; returns after user quits */
static void displayTable(Student *arr, int n, SortKey key, const char *title) {
    if (n == 0) { printWarning("No records to display."); return; }

    sortArr(arr, n, key);

    int page = 0;
    int pages = (n + PAGE_SIZE - 1) / PAGE_SIZE;

    while (1) {
        clearScreen();
        printf("\n");
        printHLine(BOX_TL, BOX_H, BOX_TR);
        printBoxTitle(title);
        printHLine(BOX_ML, BOX_H, BOX_MR);
        /* Status line */
        char status[BOX_WIDTH];
        snprintf(status, sizeof(status),
                 "  Total: %d student%s   Page %d of %d",
                 n, n == 1 ? "" : "s", page + 1, pages);
        printf(BOX_V " " C_DIM "%-*s" C_RESET BOX_V "\n",
               BOX_WIDTH - 3, status);
        printHLine(BOX_ML, BOX_H, BOX_MR);

        printTableHeader();

        int start = page * PAGE_SIZE;
        int end   = start + PAGE_SIZE;
        if (end > n) end = n;
        for (int i = start; i < end; i++)
            printTableRow(&arr[i], i - start);

        printTableFooter();

        /* Navigation */
        printf("\n  " C_DIM);
        if (page > 0)      printf("[P] Prev  ");
        if (page < pages - 1) printf("[N] Next  ");
        printf("[Q] Back" C_RESET "   ");
        fflush(stdout);

        char ch[8];
        if (fgets(ch, sizeof(ch), stdin)) {
            char c = tolower((unsigned char)ch[0]);
            if (c == 'q') break;
            if (c == 'n' && page < pages - 1) page++;
            if (c == 'p' && page > 0)         page--;
        }
    }
}

/* ── Public API ──────────────────────────────────────────── */

int countStudents(void) {
    FILE *f = fopen(DATA_FILE, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    int n = (int)(ftell(f) / sizeof(Student));
    fclose(f);
    return n;
}

float avgGPA(void) {
    Student *arr;
    int n = loadAll(&arr);
    if (n == 0) return 0.0f;
    float sum = 0;
    for (int i = 0; i < n; i++) sum += arr[i].gpa;
    free(arr);
    return sum / n;
}

/* ── Add ─────────────────────────────────────────────────── */

void addStudent(void) {
    Student *arr;
    int n = loadAll(&arr);

    Student s;
    memset(&s, 0, sizeof(s));
    s.id = nextId(arr, n);
    free(arr);

    clearScreen();
    printf("\n");
    printHLine(BOX_TL, BOX_H, BOX_TR);
    printBoxTitle("ADD NEW STUDENT");
    printHLine(BOX_ML, BOX_H, BOX_MR);
    printBoxBlank();
    printf(BOX_V "  " C_DIM "Auto-assigned ID: " C_BWHITE "%d" C_RESET
           "%*s" BOX_V "\n", s.id, BOX_WIDTH - 24 - (s.id > 999 ? 4 : s.id > 99 ? 3 : s.id > 9 ? 2 : 1), "");
    printBoxBlank();
    printHLine(BOX_ML, BOX_H, BOX_MR);
    printBoxBlank();
    printHLine(BOX_BL, BOX_H, BOX_BR);
    printf("\n");

    readString("Full Name        : ", s.name,   MAX_NAME);
    s.age = readInt("Age (15–100)     : ", 15, 100);
    readString("Course           : ", s.course, MAX_COURSE);
    s.gpa = readFloat("GPA (0.00–4.00)  : ", 0.0f, 4.0f);
    readString("Email            : ", s.email,  MAX_EMAIL);
    readString("Phone            : ", s.phone,  MAX_PHONE);

    time_t t = time(NULL);
    int curYear = (int)(localtime(&t)->tm_year + 1900);
    s.year = readInt("Enrollment Year  : ", 1990, curYear);

    /* Append */
    FILE *f = fopen(DATA_FILE, "ab");
    if (!f) { printError("Could not open data file."); return; }
    fwrite(&s, sizeof(Student), 1, f);
    fclose(f);

    printSuccess("Student added successfully!");
    printf("  " C_DIM "ID: %d  |  %s  |  %s  |  GPA: %.2f\n" C_RESET,
           s.id, s.name, s.course, s.gpa);
}

/* ── View ────────────────────────────────────────────────── */

void viewStudents(SortKey key) {
    Student *arr;
    int n = loadAll(&arr);
    if (n == 0) { printWarning("No students on record."); pauseScreen(); return; }
    displayTable(arr, n, key, "ALL STUDENTS");
    free(arr);
}

/* ── Search ──────────────────────────────────────────────── */

void searchStudent(void) {
    Student *arr;
    int n = loadAll(&arr);
    if (n == 0) { printWarning("No students on record."); pauseScreen(); return; }

    clearScreen();
    printf("\n");
    printHLine(BOX_TL, BOX_H, BOX_TR);
    printBoxTitle("SEARCH STUDENTS");
    printHLine(BOX_ML, BOX_H, BOX_MR);
    printf(BOX_V "  " C_CYAN "[1]" C_RESET " Search by ID"
           "%*s" BOX_V "\n", BOX_WIDTH - 17, "");
    printf(BOX_V "  " C_CYAN "[2]" C_RESET " Search by Name (partial)"
           "%*s" BOX_V "\n", BOX_WIDTH - 29, "");
    printf(BOX_V "  " C_CYAN "[3]" C_RESET " Search by Course"
           "%*s" BOX_V "\n", BOX_WIDTH - 21, "");
    printHLine(BOX_BL, BOX_H, BOX_BR);
    printf("\n");

    int choice = readInt("Choice [1-3]: ", 1, 3);
    printf("\n");

    Student *results = malloc(n * sizeof(Student));
    int rn = 0;

    if (choice == 1) {
        int id = readInt("Student ID: ", 1, 999999);
        for (int i = 0; i < n; i++)
            if (arr[i].id == id) results[rn++] = arr[i];
    } else if (choice == 2) {
        char query[64];
        readString("Name (partial): ", query, sizeof(query));
        for (int i = 0; i < n; i++)
            if (containsCI(arr[i].name, query)) results[rn++] = arr[i];
    } else {
        char query[64];
        readString("Course (partial): ", query, sizeof(query));
        for (int i = 0; i < n; i++)
            if (containsCI(arr[i].course, query)) results[rn++] = arr[i];
    }

    free(arr);

    if (rn == 0) {
        printWarning("No matching students found.");
        pauseScreen();
        free(results);
        return;
    }

    displayTable(results, rn, SORT_ID, "SEARCH RESULTS");
    free(results);
}

/* ── Update ──────────────────────────────────────────────── */

void updateStudent(void) {
    Student *arr;
    int n = loadAll(&arr);
    if (n == 0) { printWarning("No students on record."); pauseScreen(); return; }

    printf("\n");
    int id = readInt("Enter Student ID to update: ", 1, 999999);

    int idx = -1;
    for (int i = 0; i < n; i++)
        if (arr[i].id == id) { idx = i; break; }

    if (idx < 0) {
        printError("Student not found.");
        free(arr);
        pauseScreen();
        return;
    }

    Student *s = &arr[idx];
    printf("\n  " C_DIM "Leave blank to keep current value.\n\n" C_RESET);

    char prompt[128];
    snprintf(prompt, sizeof(prompt), "Name   [%s]: ", s->name);
    readStringOpt(prompt, s->name, MAX_NAME);

    printf("  Age    [%d]: ", s->age);
    fflush(stdout);
    char tmp[32]; int parsed;
    if (fgets(tmp, sizeof(tmp), stdin) && sscanf(tmp, "%d", &parsed) == 1)
        if (parsed >= 15 && parsed <= 100) s->age = parsed;

    snprintf(prompt, sizeof(prompt), "Course [%s]: ", s->course);
    readStringOpt(prompt, s->course, MAX_COURSE);

    printf("  GPA    [%.2f]: ", s->gpa);
    fflush(stdout);
    float fp;
    if (fgets(tmp, sizeof(tmp), stdin) && sscanf(tmp, "%f", &fp) == 1)
        if (fp >= 0.0f && fp <= 4.0f) s->gpa = fp;

    snprintf(prompt, sizeof(prompt), "Email  [%s]: ", s->email);
    readStringOpt(prompt, s->email, MAX_EMAIL);

    snprintf(prompt, sizeof(prompt), "Phone  [%s]: ", s->phone);
    readStringOpt(prompt, s->phone, MAX_PHONE);

    printf("  Year   [%d]: ", s->year);
    fflush(stdout);
    if (fgets(tmp, sizeof(tmp), stdin) && sscanf(tmp, "%d", &parsed) == 1)
        if (parsed >= 1990 && parsed <= 2100) s->year = parsed;

    if (saveAll(arr, n))
        printSuccess("Student record updated successfully.");
    else
        printError("Failed to save changes.");

    free(arr);
}

/* ── Delete ──────────────────────────────────────────────── */

void deleteStudent(void) {
    Student *arr;
    int n = loadAll(&arr);
    if (n == 0) { printWarning("No students on record."); pauseScreen(); return; }

    printf("\n");
    int id = readInt("Enter Student ID to delete: ", 1, 999999);

    int idx = -1;
    for (int i = 0; i < n; i++)
        if (arr[i].id == id) { idx = i; break; }

    if (idx < 0) {
        printError("Student not found.");
        free(arr);
        pauseScreen();
        return;
    }

    Student *s = &arr[idx];
    printf("\n  " C_BYELLOW "Student to delete:" C_RESET "\n");
    printf("  ID: %d  |  %s  |  %s  |  GPA: %.2f\n\n",
           s->id, s->name, s->course, s->gpa);
    printf("  " C_BRED "Confirm delete? [y/N]: " C_RESET);
    fflush(stdout);

    char ch[8];
    if (!fgets(ch, sizeof(ch), stdin) || tolower((unsigned char)ch[0]) != 'y') {
        printInfo("Delete cancelled.");
        free(arr);
        pauseScreen();
        return;
    }

    /* Shift records */
    for (int i = idx; i < n - 1; i++) arr[i] = arr[i + 1];
    n--;

    if (saveAll(arr, n))
        printSuccess("Student deleted successfully.");
    else
        printError("Failed to save changes.");

    free(arr);
}

/* ── Sort menu ───────────────────────────────────────────── */

void sortMenu(void) {
    clearScreen();
    printf("\n");
    printHLine(BOX_TL, BOX_H, BOX_TR);
    printBoxTitle("SORT & VIEW STUDENTS");
    printHLine(BOX_ML, BOX_H, BOX_MR);
    printf(BOX_V "  " C_CYAN "[1]" C_RESET " By ID              "
                  C_CYAN "[4]" C_RESET " By GPA (Low → High)"
           "  " BOX_V "\n");
    printf(BOX_V "  " C_CYAN "[2]" C_RESET " By Name (A → Z)    "
                  C_CYAN "[5]" C_RESET " By Age             "
           "  " BOX_V "\n");
    printf(BOX_V "  " C_CYAN "[3]" C_RESET " By GPA (High → Low)"
                  C_CYAN "[6]" C_RESET " By Course          "
           "  " BOX_V "\n");
    printf(BOX_V "  " C_CYAN "[7]" C_RESET " By Enrollment Year "
           "%*s" BOX_V "\n", BOX_WIDTH - 30, "");
    printHLine(BOX_BL, BOX_H, BOX_BR);
    printf("\n");

    int c = readInt("Sort by [1-7]: ", 1, 7);
    SortKey keys[] = {SORT_ID, SORT_NAME, SORT_GPA_DESC,
                      SORT_GPA_ASC, SORT_AGE, SORT_COURSE, SORT_YEAR};
    viewStudents(keys[c - 1]);
}

/* ── Statistics ──────────────────────────────────────────── */

void statistics(void) {
    Student *arr;
    int n = loadAll(&arr);

    clearScreen();
    printf("\n");
    printHLine(BOX_TL, BOX_H, BOX_TR);
    printBoxTitle("STATISTICS REPORT");
    printHLine(BOX_ML, BOX_H, BOX_MR);

    if (n == 0) {
        printBoxBlank();
        printBoxTitle("No student records found.");
        printBoxBlank();
        printHLine(BOX_BL, BOX_H, BOX_BR);
        pauseScreen();
        return;
    }

    float sum = 0, hi = arr[0].gpa, lo = arr[0].gpa;
    int hiIdx = 0, loIdx = 0;
    int gradeCount[5] = {0};   /* A B C D F */

    for (int i = 0; i < n; i++) {
        sum += arr[i].gpa;
        if (arr[i].gpa > hi) { hi = arr[i].gpa; hiIdx = i; }
        if (arr[i].gpa < lo) { lo = arr[i].gpa; loIdx = i; }
        if      (arr[i].gpa >= 3.5f) gradeCount[0]++;
        else if (arr[i].gpa >= 3.0f) gradeCount[1]++;
        else if (arr[i].gpa >= 2.0f) gradeCount[2]++;
        else if (arr[i].gpa >= 1.0f) gradeCount[3]++;
        else                         gradeCount[4]++;
    }
    float avg = sum / n;

    /* Summary */
    printBoxBlank();
    printf(BOX_V "  " C_BOLD "Total Students  " C_RESET ": "
           C_BWHITE "%d" C_RESET
           "%*s" BOX_V "\n", n, BOX_WIDTH - 22 - (n > 99 ? 3 : n > 9 ? 2 : 1), "");

    printf(BOX_V "  " C_BOLD "Average GPA     " C_RESET ": "
           "%s%.2f" C_RESET " (%s)"
           "%*s" BOX_V "\n",
           gpaColor(avg), avg, gpaGrade(avg),
           BOX_WIDTH - 30, "");

    printf(BOX_V "  " C_BOLD "Highest GPA     " C_RESET ": "
           C_BGREEN "%.2f" C_RESET " — %s (%s)"
           "%*s" BOX_V "\n",
           hi, arr[hiIdx].name, arr[hiIdx].course,
           (int)(BOX_WIDTH - 27 - strlen(arr[hiIdx].name) - strlen(arr[hiIdx].course)), "");

    printf(BOX_V "  " C_BOLD "Lowest GPA      " C_RESET ": "
           C_BRED "%.2f" C_RESET " — %s (%s)"
           "%*s" BOX_V "\n",
           lo, arr[loIdx].name, arr[loIdx].course,
           (int)(BOX_WIDTH - 27 - strlen(arr[loIdx].name) - strlen(arr[loIdx].course)), "");

    printBoxBlank();
    printHLine(BOX_ML, BOX_H, BOX_MR);
    printBoxBlank();

    /* GPA Distribution bar chart */
    printf(BOX_V "  " C_BOLD C_BWHITE "GPA Distribution" C_RESET
           "%*s" BOX_V "\n", BOX_WIDTH - 19, "");
    printBoxBlank();

    const char *labels[]  = {"A  (≥ 3.5)", "B  (≥ 3.0)", "C  (≥ 2.0)", "D  (≥ 1.0)", "F  (< 1.0)"};
    const char *barCols[] = {C_BGREEN, C_BBLUE, C_BYELLOW, C_YELLOW, C_BRED};
    int maxBar = 30;

    for (int g = 0; g < 5; g++) {
        int cnt   = gradeCount[g];
        int bars  = (n > 0) ? (cnt * maxBar / n) : 0;
        int pct   = (n > 0) ? (cnt * 100 / n)   : 0;

        printf(BOX_V "  %s%-10s" C_RESET " %s",
               barCols[g], labels[g], barCols[g]);
        for (int b = 0; b < bars; b++) printf("█");
        printf(C_RESET C_DIM " %d (%d%%)" C_RESET,  cnt, pct);

        /* right-pad to box width */
        int used = 2 + 10 + 1 + bars + 2 + (cnt > 9 ? 2 : 1) + 6;
        int pad  = BOX_WIDTH - 2 - used;
        if (pad < 0) pad = 0;
        printf("%*s" BOX_V "\n", pad, "");
    }

    printBoxBlank();
    printHLine(BOX_BL, BOX_H, BOX_BR);

    free(arr);
    pauseScreen();
}

/* ── Export ──────────────────────────────────────────────── */

void exportCSV(void) {
    Student *arr;
    int n = loadAll(&arr);
    if (n == 0) { printWarning("No records to export."); pauseScreen(); return; }

    const char *path = "students_export.csv";
    FILE *f = fopen(path, "w");
    if (!f) { printError("Could not create export file."); free(arr); pauseScreen(); return; }

    fprintf(f, "ID,Name,Age,Course,GPA,Email,Phone,EnrollmentYear\n");
    for (int i = 0; i < n; i++) {
        Student *s = &arr[i];
        fprintf(f, "%d,\"%s\",%d,\"%s\",%.2f,\"%s\",\"%s\",%d\n",
                s->id, s->name, s->age, s->course,
                s->gpa, s->email, s->phone, s->year);
    }
    fclose(f);
    free(arr);

    printSuccess("Export complete!");
    printf("  " C_DIM "Saved to: " C_BCYAN "%s" C_RESET
           C_DIM "  (%d record%s)\n" C_RESET,
           path, n, n == 1 ? "" : "s");
    pauseScreen();
}
