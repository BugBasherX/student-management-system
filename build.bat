@echo off
echo Building Student Management System...
gcc -Wall -Wextra -std=c99 -Iinclude src/main.c src/login.c src/student.c src/ui.c src/backup.c src/sha256.c -o student_mgmt.exe

if %ERRORLEVEL% equ 0 (
    echo [OK] Build successful: student_mgmt.exe
    echo Run with: .\student_mgmt.exe
) else (
    echo [ERROR] Build failed. Please ensure GCC is installed and on your PATH.
)
