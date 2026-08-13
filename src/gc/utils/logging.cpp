#include "logging.h"
#include <cstdarg>
#include <cstdio>

namespace CS2GC {

void Log(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    printf("[GC] %s\n", buffer);
    fflush(stdout);
}

}