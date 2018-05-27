#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "helpers/testing.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int __test_abort_count;

void __emq_abort(const char* message, ...) {
    va_list args;
    va_start(args, message);
    printf("\n\t>> Captured " ANSI_COLOR_RED "abort: " ANSI_COLOR_MAGENTA);
    vprintf(message, args);
    printf(ANSI_COLOR_RESET "\n");
    va_end(args);

    ++__test_abort_count;
}

void clearAbort(void) {
    __test_abort_count = 0;
}

int hasAbort(void) {
    if (__test_abort_count == 1) {
        return 1;
    } else if (__test_abort_count == 0) {
        return 0;
    } else {
        printf("\nERROR: hasAbort detected %d aborts. Have you called clearAbort?\n", __test_abort_count);
        abort();
    }
}

int getAbortsCount(void) {
    return __test_abort_count;
}
