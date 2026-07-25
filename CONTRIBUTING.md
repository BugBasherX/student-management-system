# Contributing — Student Management System v2.0

Guidelines for building, extending, and maintaining this project.

---

## Build Setup

### Requirements

- GCC with C99 support
- A POSIX-compatible system (Linux, macOS, or Windows WSL)
- A UTF-8 terminal

### Compile

```bash
gcc -o student_mgmt main.c login.c student.c ui.c backup.c
```

### Compile with warnings (recommended while developing)

```bash
gcc -Wall -Wextra -std=c99 -o student_mgmt main.c login.c student.c ui.c backup.c
```

### Run

```bash
./student_mgmt
```

### Clean

```bash
rm -f student_mgmt students.dat admin.cfg students_export.csv
rm -rf backups/
```

---

## Code Style

| Rule | Example |
|------|---------|
| Standard | C99 — avoid compiler-specific extensions |
| Naming | `snake_case` for all functions and variables |
| Macros | `UPPER_CASE` for constants and macros |
| Braces | Opening brace on the same line as the statement |
| Indentation | 4 spaces — no tabs |
| Line length | Keep lines under 100 characters where practical |
| Comments | Use `/* … */` block comments for sections, `//` for inline |
| No global state | Do not introduce global variables; pass data through parameters |
| No raw scanf | All user input must go through `ui.c` helpers (`readInt`, `readString`, etc.) |
| No exit() in modules | Only `main.c` may call `exit()` — library functions return error codes or print and return |

---

## Project Structure Rules

- **`ui.c / ui.h`** — terminal/display only. No business logic, no file I/O.
- **`login.c / login.h`** — authentication only. No student data access.
- **`student.c / student.h`** — student data operations only. May call `ui.c` helpers.
- **`backup.c / backup.h`** — file copy operations only. May call `ui.c` helpers and `countStudents()`.
- **`main.c`** — orchestration only. No direct data manipulation; calls module functions.

---

## Adding a New Feature

### Step 1 — Write the function

Add it to the most appropriate existing `.c` file, or create a new `feature.c / feature.h` pair.

```c
/* feature.h */
#ifndef FEATURE_H
#define FEATURE_H
void myFeature(void);
#endif

/* feature.c */
#include "feature.h"
#include "ui.h"

void myFeature(void) {
    clearScreen();
    printf("\n");
    printHLine(BOX_TL, BOX_H, BOX_TR);
    printBoxTitle("MY FEATURE");
    printHLine(BOX_BL, BOX_H, BOX_BR);
    /* ... */
    pauseScreen();
}
```

### Step 2 — Add it to the menu

In `main.c`:

```c
/* In displayMainMenu() — add a printf line */
printf(BOX_V "  " C_CYAN "[13]" C_RESET " My Feature"
       "%*s" BOX_V "\n", BOX_WIDTH - 19, "");

/* In the switch statement */
case 13: myFeature(); break;
```

### Step 3 — Add the new file to the build command

```bash
gcc -Wall -Wextra -o student_mgmt main.c login.c student.c ui.c backup.c feature.c
```

---

## Adding a New Student Field

1. Add the field to the `Student` struct in `student.h`:
   ```c
   char department[64];
   ```

2. Add the input prompt in `addStudent()` in `student.c`:
   ```c
   readString("Department : ", s.department, 64);
   ```

3. Add the optional-update prompt in `updateStudent()`:
   ```c
   snprintf(prompt, sizeof(prompt), "Dept   [%s]: ", s->department);
   readStringOpt(prompt, s->department, 64);
   ```

4. Add the field to `exportCSV()` — both the CSV header and the data row.

5. **Delete `students.dat`** — the binary struct layout changed; old data is incompatible.

---

## Box Layout Rules

All output must fit within `BOX_WIDTH = 78` columns (76 inner characters between the two `║` borders).

When adding a new box screen:

```c
clearScreen();
printf("\n");
printHLine(BOX_TL, BOX_H, BOX_TR);   /* top border    */
printBoxTitle("MY SCREEN TITLE");
printHLine(BOX_ML, BOX_H, BOX_MR);   /* mid separator */
printBoxBlank();                       /* blank row     */
/* content rows — each must be exactly BOX_WIDTH chars wide */
printf(BOX_V " %-*s " BOX_V "\n", BOX_WIDTH - 4, "some text");
printBoxBlank();
printHLine(BOX_BL, BOX_H, BOX_BR);   /* bottom border */
```

---

## Common Pitfalls

| Problem | Cause | Fix |
|---------|-------|-----|
| Students lost after recompile | Struct layout changed | Delete `students.dat` and re-enter data |
| Box borders misaligned | printf width arithmetic wrong | Count inner chars: must total `BOX_WIDTH - 2` |
| Password masking not working | Running on Windows without WSL | Use WSL or stub out `termios` calls |
| Garbled box characters | Terminal not set to UTF-8 | Set `LANG=en_US.UTF-8` or use a UTF-8 terminal |
| Partial backup name shown | `strncpy` missing null terminator | Always set `buf[maxlen-1] = '\0'` after strncpy |

---

## Checklist Before Committing

- [ ] Compiles cleanly with `-Wall -Wextra` and zero warnings
- [ ] All user input goes through `ui.c` helpers
- [ ] Every `loadAll()` is matched with `free()`
- [ ] New box screens stay within `BOX_WIDTH = 78`
- [ ] `pauseScreen()` is called at the end of every terminal operation
- [ ] `ARCHITECTURE.md` updated if struct or module boundaries changed
- [ ] `CHANGELOG.md` updated with a short entry under the next version
