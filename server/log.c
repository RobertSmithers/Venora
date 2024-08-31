#include "log.h"

#define LOG_FILE "server.log"

static const char *get_level_str(LogLevel level)
{
    switch (level)
    {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARN:
        return "WARN";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

void log_message(LogLevel level, const char *func, const char *format, ...)
{
    time_t now = time(NULL);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    #ifndef DEBUG // Log to file
    FILE *log_file = fopen(LOG_FILE, "a");
    if (!log_file)
    {
        perror("Failed to open log file");
        return;
    }
    fprintf(log_file, "[%s] [%s] [%s] ", time_str, get_level_str(level), func);

    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);

    fprintf(log_file, "\n");

    fclose(log_file);

    #else // Not DEBUG (print to stdout)
    printf("[%s] [%s] [%s] ", time_str, get_level_str(level), func);

    #endif // DEBUG
}
