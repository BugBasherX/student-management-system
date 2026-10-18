# Student Management System - Makefile
CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99 -Iinclude

SRCS = src/main.c src/login.c src/student.c src/ui.c src/backup.c src/sha256.c
OBJS = $(SRCS:.c=.o)
TARGET = student_mgmt

TEST_SRCS = tests/test_all.c src/sha256.c src/student.c src/ui.c src/backup.c
TEST_TARGET = test_runner

ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    TEST_TARGET := $(TEST_TARGET).exe
    RM = cmd /C del /Q /F
    RMDIR = cmd /C rmdir /S /Q
else
    RM = rm -f
    RMDIR = rm -rf
endif

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
ifeq ($(OS),Windows_NT)
	.\$(TARGET)
else
	./$(TARGET)
endif

$(TEST_TARGET): $(TEST_SRCS)
	$(CC) $(CFLAGS) -o $@ $(TEST_SRCS)

test: $(TEST_TARGET)
ifeq ($(OS),Windows_NT)
	.\$(TEST_TARGET)
else
	./$(TEST_TARGET)
endif

clean:
	$(RM) $(TARGET) $(TEST_TARGET) src\*.o src/*.o 2>nul || true
	$(RM) students.dat admin.cfg students_export.csv 2>nul || true
	$(RMDIR) backups 2>nul || true
