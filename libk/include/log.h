#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include "../../include/io/serial.h"

typedef enum {
    DEBUG,
    LOG,
    ERROR
} LogLevel;

#define log_debug(fmt, ...) (log_log(DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__))
#define log(fmt, ...) (log_log(LOG, __FILE__, __LINE__, fmt, ##__VA_ARGS__))
#define log_error(fmt, ...) (log_log(ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__))

void log_log(LogLevel level, const char* file_name, int line_no, const char* fmt, ...);
#endif
