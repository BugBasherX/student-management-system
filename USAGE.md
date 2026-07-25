# Usage Guide — Student Management System v2.0

Complete walkthrough of every feature in the application.

---

## Table of Contents

1. [Starting the Program](#1-starting-the-program)
2. [Logging In](#2-logging-in)
3. [Main Menu](#3-main-menu)
4. [Add Student](#4-add-student)
5. [View All Students](#5-view-all-students)
6. [Search Student](#6-search-student)
7. [Update Student](#7-update-student)
8. [Delete Student](#8-delete-student)
9. [Sort & View](#9-sort--view)
10. [Statistics Report](#10-statistics-report)
11. [Export to CSV](#11-export-to-csv)
12. [Top-N Leaderboard](#12-top-n-leaderboard)
13. [Course Roster](#13-course-roster)
14. [Backup & Restore](#14-backup--restore)
15. [Change Password](#15-change-password)
16. [Logging Out & Quitting](#16-logging-out--quitting)

---

## 1. Starting the Program

Compile once, then run.

**macOS / Linux / WSL:**
```bash
gcc -o student_mgmt main.c login.c student.c ui.c backup.c && ./student_mgmt
```

**Windows — PowerShell** (`&&` is not valid — use two lines):
```powershell
gcc -o student_mgmt.exe main.c login.c student.c ui.c backup.c
./student_mgmt.exe
```

**Windows — cmd.exe:**
```cmd
gcc -o student_mgmt.exe main.c login.c student.c ui.c backup.c && student_mgmt.exe
```

The login screen appears immediately.

---

## 2. Logging In

```
Username : admin
Password : admin123   ← shown as ********* while typing
```

- Password input is **masked** — each character shows as `*`.
- Backspace works normally during password entry.
- **3 failed attempts** locks the session and exits the program.
- Your password can be changed from the main menu (option 12).

---

## 3. Main Menu

After login you see a status bar showing your username, total student count, and current average GPA. The 12 options are laid out in two columns:

```
[ 1] Add Student          [ 7] Statistics Report
[ 2] View All Students    [ 8] Export to CSV
[ 3] Search Student       [ 9] Top-N Leaderboard
[ 4] Update Student       [10] Course Roster
[ 5] Delete Student       [11] Backup & Restore
[ 6] Sort & View          [12] Change Password

[ 0] Logout    [Q] Quit
```

Type the number and press Enter. Type `Q` and press Enter to quit.

---

## 4. Add Student

**Menu → 1**

You are prompted for:

| Field | Validation |
|-------|-----------|
| Full Name | Cannot be empty |
| Age | Must be 15 – 100 |
| Course | Cannot be empty |
| GPA | Must be 0.00 – 4.00 |
| Email | Cannot be empty |
| Phone | Cannot be empty |
| Enrollment Year | Must be 1990 – current year |

The student ID is assigned automatically (next available integer).  
After saving, the new ID, name, course, and GPA are confirmed on screen.

---

## 5. View All Students

**Menu → 2**

Displays all students sorted by ID in a paginated table, 10 rows per page.

**Navigation:**
| Key | Action |
|-----|--------|
| `N` | Next page |
| `P` | Previous page |
| `Q` | Back to main menu |

**GPA colour coding:**

| Colour | Range |
|--------|-------|
| 🟢 Green | 3.50 – 4.00 |
| 🟡 Yellow | 2.50 – 3.49 |
| 🟠 Orange | 1.00 – 2.49 |
| 🔴 Red | 0.00 – 0.99 |

---

## 6. Search Student

**Menu → 3**

Choose a search mode:

| Option | Searches by | Match type |
|--------|-------------|------------|
| 1 | Student ID | Exact |
| 2 | Name | Partial, case-insensitive |
| 3 | Course | Partial, case-insensitive |

Results are shown in the same paginated table. For example, searching name `ali` will match "Alice Johnson" and "Malik Ahmed".

---

## 7. Update Student

**Menu → 4**

1. Enter the student ID you want to edit.
2. Each field shows its **current value** in brackets.
3. Press **Enter** (blank) to keep the current value, or type a new one.

Example:
```
Name   [Alice Johnson]:          ← pressed Enter, kept unchanged
Age    [20]: 21                  ← updated to 21
Course [Computer Science]:       ← kept unchanged
GPA    [3.85]: 3.90              ← updated
```

Changes are saved immediately after all fields are reviewed.

---

## 8. Delete Student

**Menu → 5**

1. Enter the student ID to delete.
2. The full record is displayed for review.
3. Type `y` and press Enter to confirm. Anything else cancels.

Deletion is permanent — use **Backup** (option 11) beforehand if needed.

---

## 9. Sort & View

**Menu → 6**

Choose a sort column:

| Option | Sorted by |
|--------|-----------|
| 1 | ID (default) |
| 2 | Name A → Z |
| 3 | GPA High → Low |
| 4 | GPA Low → High |
| 5 | Age |
| 6 | Course A → Z |
| 7 | Enrollment Year |

The sorted result is shown in the same paginated table.

---

## 10. Statistics Report

**Menu → 7**

Displays a full report:

- **Total students** on record
- **Average GPA** with letter grade
- **Highest GPA** — student name and course
- **Lowest GPA** — student name and course
- **Grade distribution** bar chart:
  - A (≥ 3.5), B (≥ 3.0), C (≥ 2.0), D (≥ 1.0), F (< 1.0)
  - Each bar shows count and percentage

---

## 11. Export to CSV

**Menu → 8**

Exports all student records to `students_export.csv` in the project folder.

**CSV columns:**
```
ID, Name, Age, Course, GPA, Email, Phone, EnrollmentYear
```

The file can be opened in Excel, Google Sheets, or any spreadsheet application.

---

## 12. Top-N Leaderboard

**Menu → 9**

1. Enter how many top students to show (1 – 50).
2. Students are ranked by GPA, highest first.
3. Top 3 get special medals:
   - 🥇 Rank 1 — Gold
   - 🥈 Rank 2 — Silver
   - 🥉 Rank 3 — Bronze

Useful for end-of-semester honour roll generation.

---

## 13. Course Roster

**Menu → 10**

1. Enter a course name (partial match, case-insensitive).  
   Example: typing `comp` matches "Computer Science", "Computing", etc.
2. All matching students are shown in a paginated table, sorted by GPA (high to low).
3. A summary panel follows showing:
   - Total enrolled in that course
   - Average GPA
   - Highest and lowest GPA with student names

---

## 14. Backup & Restore

**Menu → 11**

### Create Backup
Copies `students.dat` to `backups/students_YYYYMMDD_HHMMSS.dat`.  
Backups are sorted chronologically. The `backups/` folder is created automatically.

### Restore from Backup
1. A numbered list of available backups is shown (newest first).
2. Enter the number of the backup to restore.
3. A confirmation prompt is shown before overwriting.
4. The current data is **automatically backed up** before the restore overwrites it.

> Tip: create a backup before any bulk update or delete session.

---

## 15. Change Password

**Menu → 12**

1. Enter your **current password** (masked).
2. Enter a **new password** (masked) — minimum 4 characters.
3. Enter the new password again to confirm.

The new password is saved to `admin.cfg` and takes effect immediately.  
The default password `admin123` is used if `admin.cfg` does not exist.

---

## 16. Logging Out & Quitting

| Input | Action |
|-------|--------|
| `0` | Logout — returns to the login screen |
| `Q` | Quit — exits the program entirely |

Logging out allows a different admin session to begin without restarting.
