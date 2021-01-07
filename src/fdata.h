#ifndef FDATA_H_
#define FDATA_H_

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
FILE *open_file(const char *path);

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
int count_lines(FILE *fptr);

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
int load_points_from_file(const b_point *points, const int count, FILE *fptr);

/**
 * Function: process_file
 * ----------------------
 * Read a file at the given file path. Can be a relative or absolute path.
 * 
 * path: Relative or absolute string file path to the file to load.
 * 
 * returns: 0 if successful, -1 if unsuccessful.
 * 
 **/
int process_file(const char *path);

#endif
