#ifndef PLAYGROUND_LOG_H
#define PLAYGROUND_LOG_H

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#ifndef LOGGING_STREAM
#define LOGGING_STREAM stderr
#endif

#ifdef LOGGING_FILE_NAME_ONLY
#define LOGGING_FILE_NAME __FILE_NAME__
#else
#define LOGGING_FILE_NAME __FILE__
#endif

#ifndef NDEBUG
#define LOG_DEBUG
#endif

#define TIMESTAMP_FORMAT    "%04d-%02d-%02d %02d:%02d:%02d"
#define TIMESTAMP_ITEMS(tm) (1900 + (tm)->tm_year), (1 + (tm)->tm_mon), (tm)->tm_mday, (tm)->tm_hour, (tm)->tm_min, (tm)->tm_sec

const char *const LOG_LEVEL_NAMES[] = {
        "DEBUG",
        "ERROR"
};

#define log(level, format, ...)                             \
do {                                                        \
    time_t _timer = time(NULL);                             \
    struct tm* _t = localtime(&_timer);                     \
    fprintf(                                                \
        LOGGING_STREAM,                                     \
        TIMESTAMP_FORMAT " [%-5s] %s %s:%d " format "\n",   \
        TIMESTAMP_ITEMS(_t),                                \
        LOG_LEVEL_NAMES[(level)],                           \
        __FUNCTION__, LOGGING_FILE_NAME, __LINE__,          \
        ##__VA_ARGS__                                       \
    );                                                      \
} while (0)

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_ERROR 1

#define log_error(format, ...) log(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)

#ifdef LOG_DEBUG
#define log_debug(format, ...) log(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#else
#define log_debug(...) (void) 0
#endif

#define LOG_NAMEOF_(it)     ({ (void) it; #it; })

#define log_null(name) log_error("'%s' was %s", LOG_NAMEOF_(name), LOG_NAMEOF_(NULL))
#define log_perror(format, ...)                             \
do {                                                        \
    const typeof(errno) saved_errno = errno;                \
    const char *format_wrapper[] = {format};                \
    const size_t format_count =                             \
        sizeof(format_wrapper) / sizeof(*format_wrapper);   \
    log_error(                                              \
        format "%s%s (errno)",                              \
        format_count > 0                                    \
            ? ": "                                          \
            : ""                                            \
        ##__VA_ARGS__,                                      \
        strerror(saved_errno)                               \
    );                                                      \
} while (0)

#endif //PLAYGROUND_LOG_H
