#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

typedef enum { DEBUG, INFO, WARN, ERROR, FATAL } LogLevel;

void log_message(LogLevel level, const char *func, const char *format, ...);

#define log_debug(format, ...) log_message(DEBUG, __func__, format, ##__VA_ARGS__)
#define log_info(format, ...)  log_message(INFO,  __func__, format, ##__VA_ARGS__)
#define log_warn(format, ...)  log_message(WARN,  __func__, format, ##__VA_ARGS__)
#define log_error(format, ...) log_message(ERROR, __func__, format, ##__VA_ARGS__)
#define log_fatal(format, ...) log_message(FATAL, __func__, format, ##__VA_ARGS__)

#endif // LOG_H