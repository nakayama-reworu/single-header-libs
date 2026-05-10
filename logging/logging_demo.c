#define LOGGING_STREAM stdout
#define LOGGING_FILE_NAME_ONLY
#include "logging.h"

#include <stdlib.h>

int main(void) {
    void *ptr = NULL;
    if (NULL == ptr) {
        LogNull(ptr);
    }

    const int i = 42;
    LogDebug("i = %d", i);

    LogError("operation failed");

    errno = EBUSY;
    LogPerror("syscall #1");

    errno = 0;
    LogPerror("syscall #2");

    errno = EACCES;
    LogPerror();

    return EXIT_SUCCESS;
}
