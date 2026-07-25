/*
 * backup.c — Backup and restore for students.dat
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include "backup.h"
#include "student.h"
#include "ui.h"

#define BACKUP_DIR  "backups"
#define MAX_BACKUPS 64

/* ── Helpers ─────────────────────────────────────────────── */

static int copyFile(const char *src, const char *dst) {
    FILE *in  = fopen(src, "rb");
    if (!in) return 0;
    FILE *out = fopen(dst, "wb");
    if (!out) { fclose(in); return 0; }

    char buf[8192];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
        fwrite(buf, 1, n, out);

    fclose(in);
    fclose(out);
    return 1;
}

static void ensureBackupDir(void) {
    mkdir(BACKUP_DIR, 0755);   /* no-op if already exists */
}

/* Collect backup filenames (sorted alphabetically = chronologically) */
static int listBackups(char names[][256], int maxn) {
    DIR *d = opendir(BACKUP_DIR);
    if (!d) return 0;

    int count = 0;
    struct dirent *ent;
    while ((ent = readdir(d)) && count < maxn) {
        /* Only files that start with "students_" */
        if (strncmp(ent->d_name, "students_", 9) == 0)
            strncpy(names[count++], ent->d_name, 255);
    }
    closedir(d);

    /* Bubble sort (list is small) */
    for (int i = 0; i < count - 1; i++)
        for (int j = i + 1; j < count; j++)
            if (strcmp(names[i], names[j]) > 0) {
                char tmp[256];
                strcpy(tmp, names[i]);
                strcpy(names[i], names[j]);
                strcpy(names[j], tmp);
            }
    return count;
}

/* ── Public API ──────────────────────────────────────────── */

void createBackup(void) {
    /* Check data file exists */
    FILE *test = fopen(DATA_FILE, "rb");
    if (!test) {
        printWarning("No student data file found — nothing to back up.");
        pauseScreen();
        return;
    }
    fclose(test);

    ensureBackupDir();

    /* Timestamp-based filename */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char dst[320];
    snprintf(dst, sizeof(dst),
             BACKUP_DIR "/students_%04d%02d%02d_%02d%02d%02d.dat",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    if (copyFile(DATA_FILE, dst)) {
        printSuccess("Backup created successfully.");
        printf("  " C_DIM "File: " C_BCYAN "%s" C_RESET "\n", dst);
        printf("  " C_DIM "Records: " C_BWHITE "%d" C_RESET "\n", countStudents());
    } else {
        printError("Failed to create backup.");
    }
    pauseScreen();
}

void restoreBackup(void) {
    ensureBackupDir();

    char names[MAX_BACKUPS][256];
    int n = listBackups(names, MAX_BACKUPS);

    clearScreen();
    printf("\n");
    printHLine(BOX_TL, BOX_H, BOX_TR);
    printBoxTitle("RESTORE FROM BACKUP");
    printHLine(BOX_ML, BOX_H, BOX_MR);

    if (n == 0) {
        printBoxBlank();
        printBoxTitle("No backups found in backups/ directory.");
        printBoxBlank();
        printHLine(BOX_BL, BOX_H, BOX_BR);
        pauseScreen();
        return;
    }

    /* List backups newest-first (reverse) */
    for (int i = n - 1; i >= 0; i--) {
        int idx = n - 1 - i;   /* display index 1…n */
        printf(BOX_V "  " C_CYAN "[%2d]" C_RESET " %s%*s" BOX_V "\n",
               idx + 1, names[i],
               (int)(BOX_WIDTH - 10 - strlen(names[i])), "");
    }
    printBoxBlank();
    printHLine(BOX_BL, BOX_H, BOX_BR);
    printf("\n");

    int choice = readInt("Restore backup number (0 to cancel): ", 0, n);
    if (choice == 0) {
        printInfo("Restore cancelled.");
        pauseScreen();
        return;
    }

    /* Reverse mapping: display idx → array idx */
    int arrIdx = n - choice;
    char src[320];
    snprintf(src, sizeof(src), BACKUP_DIR "/%s", names[arrIdx]);

    printf("\n  " C_BYELLOW "⚠  This will overwrite the current student data." C_RESET "\n");
    printf("  Restoring: " C_BCYAN "%s" C_RESET "\n\n", names[arrIdx]);
    printf("  Confirm? [y/N]: ");
    fflush(stdout);

    char ch[8];
    if (!fgets(ch, sizeof(ch), stdin) ||
        (ch[0] != 'y' && ch[0] != 'Y')) {
        printInfo("Restore cancelled.");
        pauseScreen();
        return;
    }

    /* Auto-backup current data before overwriting */
    createBackup();   /* silent if no current data */

    if (copyFile(src, DATA_FILE)) {
        printSuccess("Data restored successfully.");
        printf("  " C_DIM "Students now on record: " C_BWHITE "%d" C_RESET "\n",
               countStudents());
    } else {
        printError("Restore failed — source file may be corrupted.");
    }
    pauseScreen();
}
