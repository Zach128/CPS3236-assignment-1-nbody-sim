#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "fdata.h"
#include "bpoint.h"

/**
 * Function: open_file
 * -------------------
 * Read a file at the given file path. Can be a relative or absolute path.
 * 
 * path: Relative or absolute string file path to the file to load.
 * 
 * returns: 0 if successful, -1 if unsuccessful.
 * 
 **/
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

/**
 * Function: count_lines
 * ---------------------
 * Count the lines in a file. Used for checking how many points need to be loaded.
 * 
 * fptr: A pointer to a loaded file.
 * 
 * returns: Number of lines found.
 * 
 **/
int count_lines(FILE *fptr) {
    int lines = 0;

    // Match against all newlines in the file (accounting for Windows and unix newlines).
    while (EOF != (fscanf(fptr, "%*[^\r\n]"), fscanf(fptr,"%*c")))
        ++lines;
    
    // Reset to beginning of file. Not compatible with stdin.
    rewind(fptr);

    return lines;
}

/**
 * Function: load_points_from_file
 * -------------------------------
 * Load a number of points from a given file.
 * Points are saved to a given allocated array.
 * 
 * points: A pointer to a point array to be filled.
 * count: The number of points in the file to be loaded. Must equal the total lines of the file.
 * fptr: Pointer to a loaded file.
 * 
 * returns: 0 if successful, -1 if unsuccessful.
 * 
 **/
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

int process_file(const char *path) {
    FILE *fptr = open_file(path);

    const int line_count = count_lines(fptr);
    
    printf("Found points: %i\n", line_count);

    b_point points[line_count];

    int load_result = load_points_from_file(points, line_count, fptr);

    if (load_result == -1) {
        printf("File processing failed\n");
        return -1;
    }

    printf("Printing points:\n");
    print_points(points, line_count);

    fclose(fptr);

    return 0;
}
