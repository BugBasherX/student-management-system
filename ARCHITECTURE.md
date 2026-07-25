# Architecture — Student Management System v2.0

Technical reference for the codebase: file responsibilities, data format, module boundaries, and how to extend the system.

---

## File Map

```
.
├── main.c          Entry point, login orchestration, 12-item menu loop
├── login.c / .h    Authentication, masked password input, password change
├── student.c / .h  All student operations (CRUD, search, sort, stats, export,
│                   leaderboard, roster)
├── backup.c / .h   Backup creation and restore
├── ui.c / .h       ANSI colour macros, box-drawing helpers, validated input
│
├── students.dat    Binary flat-file data store (auto-created)
├── admin.cfg       Plain-text password file (auto-created on first change)
└── backups/        Timestamped backup snapshots (auto-created)
```

---

## Module Responsibilities

### `ui.c / ui.h`

The foundation layer — nothing else in the project writes to the terminal directly except through these helpers.

**Colour macros** (`C_RESET`, `C_BGREEN`, `C_BRED`, …) — standard ANSI escape sequences, defined as string literals so they concatenate into printf format strings at compile time.

**Box-drawing constants** (`BOX_TL`, `BOX_H`, `BOX_V`, …) — UTF-8 double-line box characters.

**Key functions:**

| Function | Purpose |
|----------|---------|
| `clearScreen()` | ANSI escape `\033[2J\033[H` — clears and homes cursor |
| `pauseScreen()` | Waits for Enter; used at the end of every operation |
| `printHLine(left, fill, right)` | Prints a full-width border, e.g. `╔══…══╗` |
| `printBoxTitle(title)` | Prints a centred title inside `║ … ║` |
| `printBoxBlank()` | Prints an empty `║   …   ║` line |
| `printSuccess/Error/Warning/Info(msg)` | Coloured ✔ / ✖ / ⚠ / ℹ status lines |
| `readInt(prompt, min, max)` | Validated integer input — loops until in range |
| `readFloat(prompt, min, max)` | Validated float input |
| `readString(prompt, buf, maxlen)` | Non-empty string input — loops until non-blank |
| `readStringOpt(prompt, buf, maxlen)` | Optional string — keeps current value on blank Enter |
| `gpaColor(gpa)` | Returns ANSI colour string based on GPA value |
| `gpaGrade(gpa)` | Returns letter grade string ("A" through "F") |

`BOX_WIDTH` is defined as 78 — the total column width including the two border characters. All box lines are exactly this wide.

---

### `login.c / login.h`

**Password storage:** reads from `admin.cfg` on every login attempt. Falls back to the compiled-in default (`admin123`) if the file does not exist. This means the password survives program restarts without recompilation.

**`maskedInput(buf, maxlen)`** — disables terminal echo using `termios`, reads character by character, prints `*` for each, handles backspace with `\b \b`, restores terminal state on completion.

**`login(out_user, out_size)`** — loads the stored password, shows the login screen, collects username + masked password, returns 1 on match.

**`changePassword()`** — verifies current password (masked), reads and confirms the new password, enforces minimum length 4, writes to `admin.cfg`.

---

### `student.c / student.h`

The largest module. Internally uses two private helpers shared by all operations:

**`loadAll(Student **out)`** — opens `students.dat`, allocates a heap array, reads all records, returns count. Caller must `free()` the array.

**`saveAll(Student *arr, int n)`** — overwrites `students.dat` with the complete array.

**Sort system** — seven `qsort` comparators, selected by the `SortKey` enum:

```c
typedef enum {
    SORT_ID, SORT_NAME, SORT_GPA_DESC, SORT_GPA_ASC,
    SORT_AGE, SORT_COURSE, SORT_YEAR
} SortKey;
```

**`displayTable(arr, n, key, title)`** — the shared paginated table renderer used by View, Search, Sort, Leaderboard, and Roster. Sorts the array in-place before display.

---

### `backup.c / backup.h`

Uses `mkdir("backups", 0755)` (no-op if the directory already exists), `opendir` / `readdir` to list backup files, and a simple `copyFile()` that reads/writes in 8 KB chunks.

Backup filenames follow the pattern `students_YYYYMMDD_HHMMSS.dat` so lexicographic order equals chronological order.

---

### `main.c`

Two nested loops:

1. **Outer loop** — login phase. Calls `login()` up to `MAX_ATTEMPTS` (3) times. Exits the program on lockout.
2. **Inner loop** — menu phase. Reads a line with `fgets`, converts to integer with `atoi` (handles multi-digit choices like 10, 11, 12). `Q` exits the program; `0` breaks to the outer loop (logout).

---

## Data Format — `students.dat`

Binary flat file. Each record is a fixed-size `Student` struct written with `fwrite` and read with `fread`.

```c
typedef struct {
    int   id;           // 4 bytes  — auto-incremented, never reused
    char  name  [64];   // 64 bytes
    int   age;          // 4 bytes
    char  course[64];   // 64 bytes
    float gpa;          // 4 bytes
    char  email [64];   // 64 bytes
    char  phone [20];   // 20 bytes
    int   year;         // 4 bytes  — enrollment year
} Student;              // 228 bytes total per record
```

Record count = `file size / sizeof(Student)`.  
There is no header, no checksum, and no version field.

> **Important:** if you add, remove, or reorder fields in the struct, existing `students.dat` files become unreadable. Delete the data file and re-enter records, or write a migration utility.

---

## Control Flow Diagram

```
main()
│
├─► login loop (max 3 attempts)
│     └─► login()  ── reads admin.cfg or uses default
│
└─► menu loop
      ├─► [1]  addStudent()
      ├─► [2]  viewStudents(SORT_ID)  ──► displayTable()
      ├─► [3]  searchStudent()        ──► displayTable()
      ├─► [4]  updateStudent()
      ├─► [5]  deleteStudent()
      ├─► [6]  sortMenu()             ──► viewStudents() ──► displayTable()
      ├─► [7]  statistics()
      ├─► [8]  exportCSV()
      ├─► [9]  topStudents()          ──► displayTable()
      ├─► [10] courseRoster()         ──► displayTable()
      ├─► [11] createBackup() / restoreBackup()
      ├─► [12] changePassword()
      ├─► [0]  break  (logout → back to login loop)
      └─► [Q]  exit(0)
```

---

## How to Add a New Student Field

1. **`student.h`** — add the field to the `Student` struct.
2. **`student.c` → `addStudent()`** — add a `readString` / `readInt` / `readFloat` prompt.
3. **`student.c` → `updateStudent()`** — add a `readStringOpt` prompt with current value.
4. **`student.c` → `exportCSV()`** — add the field to the `fprintf` format string and header row.
5. **`student.c` → `printTableRow()`** — update the format string if you want it in the table (adjust column widths to keep `BOX_WIDTH = 78`).
6. **Delete `students.dat`** — the binary format changed; old records are incompatible.

---

## How to Add a New Menu Item

1. **`student.h`** (or a new header) — declare the function.
2. **Implement** the function in `student.c` (or a new `.c` file).
3. **`main.c` → `displayMainMenu()`** — add a `printf` line for the new option.
4. **`main.c` → menu `switch`** — add a `case N:` calling the function.
5. **Compile** with the new `.c` file added to the `gcc` command.

---

## Coding Conventions

| Convention | Detail |
|------------|--------|
| Standard | C99 (`gcc -std=c99` compatible) |
| Naming | `snake_case` for functions and variables, `UPPER_CASE` for macros |
| Memory | Every `loadAll()` call is paired with `free()`; no global heap state |
| Error handling | File errors print via `printError()` and return early; no `exit()` inside library functions |
| Input | All user input goes through `ui.c` helpers — never raw `scanf` in feature code |
| Width | All box output must fit within `BOX_WIDTH = 78` columns |
