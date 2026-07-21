# Student Management System

A terminal-based Student Management System written in C.

## Stack
- Language: C (C99)
- Build: GCC
- Storage: binary flat file (`students.dat`)

## How to run
The workflow compiles and launches the program in the terminal automatically.

```
gcc -o student_mgmt main.c login.c student.c && ./student_mgmt
```

## Default login credentials
- **Username:** `admin`
- **Password:** `admin123`

## Features
- Login (single admin account)
- Add / View / Search / Update / Delete students
- Student data persisted to `students.dat` between runs

## File layout
| File | Purpose |
|------|---------|
| `main.c` | Menu loop and entry point |
| `login.c` / `login.h` | Authentication |
| `student.c` / `student.h` | CRUD operations and Student struct |
| `students.dat` | Binary data store (auto-created on first add) |

## User preferences
<!-- Add personal preferences here -->
