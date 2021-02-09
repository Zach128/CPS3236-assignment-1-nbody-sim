#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdata.h"
#include "bpoint.h"

const char *out_path = "./res/out/";

FILE *open_file(const char *path, const char *mode) {
    // Open the file for read-only, checking it was successfull.
    FILE *fptr = fopen(path, mode);
    if (fptr == NULL) { 
        printf("File couldn't be opened\n");
        return NULL;
    }

    return fptr;
}

FILE *open_read_file(const char *path) {
    return open_file(path, "r");
}

FILE *open_write_file(const char *path) {
    return open_file(path, "w");
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

int load_points_from_file(b_point *points, const int count, FILE *fptr) {
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
    FILE *fptr = open_read_file(path);

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

void create_new_file(int point_count) {
    char buffer[128];
    char filepath[32768];

    // Create the file path.
    snprintf(buffer, 128, "nbody_%d.txt", point_count);
    strcpy(filepath, out_path);
    strcat(filepath, buffer);

    FILE *file = open_write_file(filepath);

    // Save a header containing the number of points to load.
    fprintf(file, "%d\n", point_count);
    fclose(file);
}

void save_points_iteration(b_point *points, int point_count)
{
    char buffer[128];
    char filepath[32768];

    // Create the file path.
    snprintf(buffer, 128, "nbody_%d.txt", point_count);
    strcpy(filepath, out_path);
    strcat(filepath, buffer);

    FILE *file = open_file(filepath, "a+");

    double m, x, y;

    for (int i = 0; i < point_count; i++)
    {
        m = points[i].mass;
        x = points[i].pos.x;
        y = points[i].pos.y;

        fprintf(file, "%.6f %.6f %.6f\n", m, x, y);
    }

    fclose(file);

    return;
}
