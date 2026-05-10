#define LOGGING_STREAM stdout
#define LOGGING_FILE_NAME_ONLY
#include "logging/logging.h"

#include <stdlib.h>

int main(void) {
    void *ptr = NULL;
    if (NULL == ptr) {
        log_null(ptr);
    }

    const int i = 42;
    log_debug("i = %d", i);

    log_error("operation failed");

    errno = EBUSY;
    log_perror("syscall #1");

    errno = 0;
    log_perror("syscall #2");

    errno = EACCES;
    log_perror();

    return EXIT_SUCCESS;
}
