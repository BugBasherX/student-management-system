#!/usr/bin/env bash
set -e

echo "Building Student Management System..."
gcc -Wall -Wextra -std=c99 -Iinclude src/main.c src/login.c src/student.c src/ui.c src/backup.c src/sha256.c -o student_mgmt

echo "[OK] Build successful: student_mgmt"
echo "Run with: ./student_mgmt"
