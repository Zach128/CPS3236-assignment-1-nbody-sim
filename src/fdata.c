#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "fdata.h"
#include "bpoint.h"

FILE *open_file(const char *path) {
    char *abs_path;
    char path_buf [PATH_MAX+1];

    // Generate an absolute path from the given one, also checking if the file exists.
    abs_path = realpath(path, path_buf);
    if (abs_path == NULL) { 
        printf("File not found: %s\nResolved path: %s\n", path, path_buf); 
        return NULL;
    }

    // Open the file for read-only, checking it was successfull.
    FILE *fptr = fopen(abs_path, "r");
    if (fptr == NULL) { 
        printf("File couldn't be opened\n");
        return NULL;
    }

    return fptr;
}

int count_lines(FILE *fptr) {
    int lines = 0;

    // Match against all newlines in the file (accounting for Windows and unix newlines).
    while (EOF != (fscanf(fptr, "%*[^\r\n]"), fscanf(fptr,"%*c")))
        ++lines;
    
    // Reset to beginning of file. Not compatible with stdin.
    rewind(fptr);

    return lines;
}

int load_points_from_file(const b_point *points, const int count, FILE *fptr) {
    int i = 0;
    
    // Load all the points by the expected pattern of mass, x, y.
    while(!feof(fptr)) {
        fscanf(fptr, "%lf, %lf, %lf", &points[i].mass, &points[i].pos.x, &points[i].pos.y);
        i++;
    }

    // Test that we loaded the same amount of points as expected.
    if (i -1 != count) {
        printf("Error: Points read do not match the expected count of points:\n\tExp: %d - Act:%d\n", count, i);
        return -1;
    }
    
    printf("Successfully read %d points.\n", i - 1);

    return 0;
}

b_point *process_file(const char *path, int *point_count) {
    FILE *fptr = open_file(path);

    // Save the number of points found.
    *point_count = count_lines(fptr);

    printf("Found points: %i\n", *point_count);

    // Allocate space for them, so we can return a pointer to this space.
    b_point *points = malloc(*point_count * sizeof(b_point));

    int load_result = load_points_from_file(points, *point_count, fptr);

    if (load_result == -1) {
        printf("File processing failed\n");
        return NULL;
    }

    fclose(fptr);

    return points;
}
