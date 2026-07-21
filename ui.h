/*
 * ui.h — ANSI colour/style macros and TUI helper declarations
 */
#ifndef UI_H
#define UI_H

/* ── Colours ─────────────────────────────────────────────── */
#define C_RESET    "\033[0m"
#define C_BOLD     "\033[1m"
#define C_DIM      "\033[2m"
#define C_RED      "\033[31m"
#define C_GREEN    "\033[32m"
#define C_YELLOW   "\033[33m"
#define C_BLUE     "\033[34m"
#define C_MAGENTA  "\033[35m"
#define C_CYAN     "\033[36m"
#define C_BRED     "\033[91m"
#define C_BGREEN   "\033[92m"
#define C_BYELLOW  "\033[93m"
#define C_BBLUE    "\033[94m"
#define C_BMAGENTA "\033[95m"
#define C_BCYAN    "\033[96m"
#define C_BWHITE   "\033[97m"

/* ── Box-drawing (UTF-8 double-line) ─────────────────────── */
#define BOX_TL "╔"
#define BOX_TR "╗"
#define BOX_BL "╚"
#define BOX_BR "╝"
#define BOX_H  "═"
#define BOX_V  "║"
#define BOX_ML "╠"
#define BOX_MR "╣"

/* Width of the outer box (including the two ║ borders) */
#define BOX_WIDTH 78

/* ── Functions ───────────────────────────────────────────── */
void clearScreen(void);
void pauseScreen(void);
void flushInput(void);

/* Print a full-width horizontal border line */
void printHLine(char *left, char *fill, char *right);

/* Print a centred title inside a box line */
void printBoxTitle(const char *title);

/* Print a blank box interior line */
void printBoxBlank(void);

/* Coloured status messages */
void printSuccess(const char *msg);
void printError  (const char *msg);
void printWarning(const char *msg);
void printInfo   (const char *msg);

/* Input helpers with validation */
int   readInt  (const char *prompt, int   min, int   max);
float readFloat(const char *prompt, float min, float max);
void  readString(const char *prompt, char *buf, int maxlen);
/* Like readString but keeps buf unchanged on empty input */
void  readStringOpt(const char *prompt, char *buf, int maxlen);

/* GPA helpers */
const char *gpaColor(float gpa);   /* ANSI colour code */
const char *gpaGrade(float gpa);   /* "A" / "B" / … / "F" */

#endif /* UI_H */
