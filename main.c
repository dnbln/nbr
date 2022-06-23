#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "nbr_driver.h"

int main(int argc, char* argv[]) {
    time_t begin = time(NULL);

    if (argc != 2) {
        printf("%s: usage: %s <file>", argv[0], argv[0]);
    }

    char *file = argv[1];

    struct stat st;
    if (stat(file, &st) == -1) {
        perror("stat");
        exit(1);
    }

    size_t sz = st.st_size;

    if (sz == 0) {
        return 0;
    }

    nbr_start(file, sz);

    time_t end = time(NULL);

    printf("\n\n%ld", end - begin);

    return 0;
}
