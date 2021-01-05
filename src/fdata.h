#ifndef FDATA_H_
#define FDATA_H_

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
