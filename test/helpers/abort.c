#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "helpers/abort.h"

int __test_abort_count;

void __emq_abort(const char* message, ...) {
    va_list args;
    va_start(args, message);
    printf("\n\t>> Captured abort: ");
    printf(message, args);
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
