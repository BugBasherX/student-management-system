# Changelog — Student Management System

All notable changes to this project are documented here.  
Format: **Added**, **Changed**, **Fixed**, **Removed**.

---

## v2.0 — Current

### Added

**New student fields**
- `email` — contact email address
- `phone` — contact phone number
- `year` — enrollment year (validated: 1990 – current year)

**Visual overhaul (`ui.c / ui.h`)**
- Full ANSI colour TUI throughout — coloured headings, GPA colour coding (green / yellow / orange / red), dimmed alternate table rows
- UTF-8 double-line box-drawing borders (`╔═╗║╚╝╠╣`) on every screen
- Coloured status messages: ✔ success (green), ✖ error (red), ⚠ warning (yellow), ℹ info (cyan)
- `BOX_WIDTH = 78` enforced globally for consistent alignment

**Login improvements (`login.c`)**
- Password masking — each character displays as `*`; backspace works correctly
- 3-attempt lockout before the program exits
- Password loaded from `admin.cfg` at runtime; falls back to compiled-in default

**Menu (12 items in `main.c`)**
- Status bar on every menu screen: signed-in username, total students, average GPA
- Two-column 12-item layout with `Q` to quit and `0` to logout
- Multi-digit input handled correctly (choices 10, 11, 12)

**Feature: Sort & View (option 6)**
- Sort by ID, Name, GPA ↓, GPA ↑, Age, Course, Enrollment Year

**Feature: Statistics Report (option 7)**
- Total count, average / highest / lowest GPA
- UTF-8 bar chart showing A / B / C / D / F grade distribution with count and percentage

**Feature: Export to CSV (option 8)**
- Writes `students_export.csv` with all 8 fields and a header row
- Compatible with Excel, Google Sheets, and any CSV reader

**Feature: Top-N Leaderboard (option 9)**
- Ranked list by GPA, configurable N (1 – 50)
- Gold / silver / bronze medal icons for ranks 1, 2, 3

**Feature: Course Roster (option 10)**
- Partial, case-insensitive course name match
- Paginated table sorted by GPA descending
- Summary panel: enrolled count, average / highest / lowest GPA

**Feature: Backup & Restore (option 11, `backup.c / backup.h`)**
- Creates timestamped snapshots: `backups/students_YYYYMMDD_HHMMSS.dat`
- Lists backups newest-first; restores by selecting a number
- Automatically backs up current data before overwriting on restore

**Feature: Change Password (option 12)**
- Masked current-password verification
- New password must be at least 4 characters; confirmed by re-entry
- Saved to `admin.cfg`; active immediately

**Paginated table (`displayTable`)**
- Shared renderer used by View, Search, Sort, Leaderboard, and Roster
- 10 rows per page; N / P / Q navigation

**Search improvements**
- Search by ID (exact), name (partial), or course (partial)
- Case-insensitive matching on name and course

**Input validation everywhere**
- Age: 15 – 100
- GPA: 0.00 – 4.00
- Year: 1990 – current year
- Name / course / email / phone: non-empty enforced
- Update: blank input keeps the current value

**Delete confirmation**
- Shows full student record before deletion
- Requires typed `y` to confirm

### Changed
- Data file extended from 5-field to 8-field `Student` struct (incompatible with v1.0 `students.dat`)
- Menu restructured: login is now separate from the main menu; Login is no longer an option — it is the entry gate
- `pauseScreen()` no longer calls `getchar()` twice (fixed double-press issue)

### Removed
- Login as a menu choice (was option 1 in v1.0) — login now gates entry to the whole program
- Web server (`server.c`) and browser frontend (`static/`) — removed in favour of the CLI

---

## v1.0 — Initial Release

### Added
- Terminal menu with 7 options: Login, Add, View, Search, Update, Delete, Exit
- Binary flat-file storage in `students.dat`
- Student fields: ID (auto), name, age, course, GPA
- Basic `printf`-based table display (no colours, no pagination)
- Single hardcoded admin account (`admin` / `admin123`)
- `students.dat` persisted between runs using `fread` / `fwrite`
