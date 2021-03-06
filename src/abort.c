#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void __emq_abort(const char* message, ...) {
    va_list args;
    va_start(args, message);
    printf("\n\nAbort: ");
    vprintf(message, args);
    va_end(args);
    abort();
}

void __emq_warning(const char* message, ...) {
    va_list args;
    va_start(args, message);
    printf("\t|Warning|");
    vprintf(message, args);
    va_end(args);
}
void __emq_debug(const char* message, ...) {
    va_list args;
    va_start(args, message);
    printf("\t:: ");
    vprintf(message, args);
    va_end(args);
}
