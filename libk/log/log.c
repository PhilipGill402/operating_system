#include <log.h>

void log_log(LogLevel level, const char* file_name, int line_no, const char* fmt, ...) {
    va_list args;
    char* log_level;

    switch(level) {
        case DEBUG:
            log_level = "DEBUG";
            break;
        case LOG:
            log_level = "LOG";
            break;
        case ERROR:
            log_level = "ERROR";
            break;
    }

    serial_printf("[%s] %s:%d: ", log_level, file_name, line_no);

    va_start(args, fmt);
    kvprintf(serial_write_char, fmt, args);
    va_end(args);
}
