# Student Management System v2.0

A terminal-based Student Management System written in C.  
Coloured TUI, paginated tables, search, sort, statistics, backup/restore, and more.

---

## Running Locally in VS Code

### Step 1 — Install the tools

| OS | What to install |
|----|-----------------|
| **Windows** | [VS Code](https://code.visualstudio.com/) + [MSYS2](https://www.msys2.org/) (provides GCC). After installing MSYS2 run `pacman -S mingw-w64-ucrt-x86_64-gcc` in the MSYS2 terminal. |
| **macOS** | [VS Code](https://code.visualstudio.com/) + run `xcode-select --install` in Terminal to get GCC. |
| **Linux** | [VS Code](https://code.visualstudio.com/) + run `sudo apt install gcc` (Ubuntu/Debian). |

Install the **C/C++** extension inside VS Code (search "C/C++" in the Extensions panel).

---

### Step 2 — Get the project

Download or clone this project, then open the folder in VS Code:

```
File → Open Folder → select the project folder
```

---

### Step 3 — Open the terminal

Inside VS Code press `` Ctrl+` `` (or **Terminal → New Terminal**).

---

### Step 4 — Compile

**macOS / Linux:**
```bash
gcc -o student_mgmt main.c login.c student.c ui.c backup.c && ./student_mgmt
```

**Windows (MSYS2 terminal):**
```bash
gcc -o student_mgmt main.c login.c student.c ui.c backup.c && ./student_mgmt
```

> ⚠️ **Windows note:** the password-masking feature uses `termios.h` which is Linux/macOS only.  
> On Windows, use **WSL** (Windows Subsystem for Linux) for full compatibility — open the MSYS2 or WSL terminal *inside* VS Code and the same command works.

---

### Step 5 — Log in

```
Username : admin
Password : admin123
```

Change your password from the main menu → **[12] Change Password**.

---

## Default credentials

| Field | Value |
|-------|-------|
| Username | `admin` |
| Password | `admin123` |

Passwords are stored in `admin.cfg` after the first change.

---

## Features

| # | Feature |
|---|---------|
| 1 | Add student (name, age, course, GPA, email, phone, year) |
| 2 | View all students — paginated, 10 per page |
| 3 | Search — by ID, partial name, or partial course |
| 4 | Update — press Enter to keep any existing value |
| 5 | Delete — shows record, asks confirmation |
| 6 | Sort & View — by ID, name, GPA↑↓, age, course, year |
| 7 | Statistics — totals, avg/hi/lo GPA, bar-chart grade distribution |
| 8 | Export to CSV — writes `students_export.csv` |
| 9 | Top-N Leaderboard — ranked list of top N students by GPA |
| 10 | Course Roster — all students in a course + per-course stats |
| 11 | Backup & Restore — timestamped snapshots in `backups/` |
| 12 | Change Password — masked input, saved to `admin.cfg` |

---

## File layout

| File | Purpose |
|------|---------|
| `main.c` | Entry point, login flow, menu loop |
| `login.c/h` | Authentication, password masking, password change |
| `student.c/h` | All student CRUD, search, sort, stats, export, leaderboard, roster |
| `backup.c/h` | Backup and restore |
| `ui.c/h` | ANSI colours, box-drawing helpers, input validators |
| `students.dat` | Binary data store (auto-created on first add) |
| `admin.cfg` | Saved password (auto-created on first password change) |
| `backups/` | Backup snapshots (auto-created on first backup) |
| `students_export.csv` | CSV export output |
