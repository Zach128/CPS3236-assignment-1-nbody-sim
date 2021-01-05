#ifndef FDATA_H_
#define FDATA_H_

// The maximum character length for each line in an input file. Equal to the byte-length of a double + 2 characters (, ) * 3.
#define MAX_POINT_S_SIZE (__SIZEOF_DOUBLE__ + 2) * 3

// Read a file at the given file path. Can be a relative or absolute path.
int process_file(const char *path);

#endif
