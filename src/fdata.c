#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

FILE *open_file(const char *path) {
    char *abs_path;
    char path_buf [PATH_MAX+1];

    // Generate an absolute path from the given one, also checking if the file exists.
    abs_path = realpath(path, path_buf);
    if (abs_path == NULL) { 
        printf("File not found: %s\nResolved path: %s\n", path, path_buf); 
        exit(0); 
    }

    // Open the file for read-only, checking it was successfull.
    FILE *fptr = fopen(abs_path, "r");
    if (fptr == NULL) { 
        printf("File couldn't be opened"); 
        exit(0);
    }

    return fptr;
}

void read_file(const char *path) {
    FILE *fptr = open_file(path);

    // Processing here

    fclose(fptr);
}
