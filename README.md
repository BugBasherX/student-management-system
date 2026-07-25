# Student Management System v2.0

A terminal-based Student Management System written in pure C.  
Coloured TUI with box-drawing borders, paginated tables, search, sort, statistics, backup/restore, and more — no external libraries required.

---

## Quick Start

### Compile & Run

**macOS / Linux / WSL (bash):**
```bash
gcc -o student_mgmt main.c login.c student.c ui.c backup.c && ./student_mgmt
```

**Windows — PowerShell** (run as two lines — `&&` is not supported in PowerShell):
```powershell
gcc -o student_mgmt.exe main.c login.c student.c ui.c backup.c
./student_mgmt.exe
```

**Windows — Command Prompt (cmd.exe)** (`&&` works here):
```cmd
gcc -o student_mgmt.exe main.c login.c student.c ui.c backup.c && student_mgmt.exe
```

**Default login:**
```
Username : admin
Password : admin123
```

---

## Features

| # | Feature | Description |
|---|---------|-------------|
| 1 | Add Student | Name, age, course, GPA, email, phone, enrollment year — all validated |
| 2 | View All | Paginated table (10 per page), navigate with N / P / Q |
| 3 | Search | By ID, partial name, or partial course name (case-insensitive) |
| 4 | Update | Edit any field; press Enter to keep the current value |
| 5 | Delete | Shows the record first, requires typed confirmation |
| 6 | Sort & View | Sort by ID, name, GPA ↑↓, age, course, or enrollment year |
| 7 | Statistics | Total count, avg / hi / lo GPA, bar-chart grade distribution |
| 8 | Export CSV | Writes `students_export.csv` with all fields |
| 9 | Top-N Leaderboard | Ranked list by GPA; gold / silver / bronze medals for top 3 |
| 10 | Course Roster | All students in a course + per-course count and GPA summary |
| 11 | Backup & Restore | Timestamped snapshots in `backups/`; auto-backup before restore |
| 12 | Change Password | Masked input, verified, saved to `admin.cfg` |

---

## Project Files

| File | Purpose |
|------|---------|
| `main.c` | Entry point, login flow, 12-item menu loop |
| `login.c / .h` | Authentication, masked password input, password change |
| `student.c / .h` | All CRUD, search, sort, stats, export, leaderboard, roster |
| `backup.c / .h` | Timestamped backup creation and restore |
| `ui.c / .h` | ANSI colours, box-drawing helpers, validated input functions |
| `students.dat` | Binary data store — auto-created on first Add |
| `admin.cfg` | Saved password — auto-created on first Change Password |
| `backups/` | Backup snapshots — auto-created on first Backup |
| `students_export.csv` | CSV export output |

---

## Documentation

| Document | Read it for… |
|----------|--------------|
| [USAGE.md](USAGE.md) | Step-by-step guide for every menu feature |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Code structure, data format, how to extend |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Build setup, code style, adding features |
| [CHANGELOG.md](CHANGELOG.md) | Version history |

---

## Requirements

- GCC (any version supporting C99)
- A UTF-8 terminal (macOS Terminal, Linux terminal, Windows Terminal)
- No external libraries

### Windows setup

1. Install [VS Code](https://code.visualstudio.com) and the **C/C++** extension
2. Install [MSYS2](https://www.msys2.org), then open the MSYS2 terminal and run:
   ```
   pacman -S mingw-w64-ucrt-x86_64-gcc
   ```
3. Add `C:\msys64\ucrt64\bin` to your system PATH
4. Open PowerShell or cmd.exe in your project folder and use the commands above

Password masking uses `conio.h` on Windows (no WSL needed — works natively).
