#ifndef _USER_H_
#define _USER_H_

/* This file contains the interface of the functions that must be implemented
 * to allow the user access to the file system and the files. IT CANNOT BE
 * MODIFIED.
 */

#define DEVICE_IMAGE "disk.dat"
#define MAX_FILE_SIZE 4096
#define MAX_UNDO 10
#define BLOCK_SIZE 4096
#define FS_SEEK_SET 0
#define FS_SEEK_BEGIN 1
#define FS_SEEK_END 2

#include "ufs.h"

/***************************/
/* File system management. */
/***************************/

/*
 * Formats a device.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int mkFS(int maxNumFiles, long deviceSize);

/*
 * Mounts a file system from the device deviceName.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int mountFS();

/*
 * Unmount file system.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int umountFS();


/*******************/
/* File read/write */
/*******************/

/*
 * Creates a new file, if it doesn't exist.
 * Returns 0 if a new file is created, 1 if the file already exists or -1 in
 * case of error.
 */
int creatFS(char *fileName);

/*
 * Opens an existing file.
 * Returns file descriptor if possible, -1 if file does not exist or -1 in case
 * of error.
 */
int openFS(char *fileName);

/*
 * Closes a file.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int closeFS(int fileDescriptor);

/*
 * Reads a number of bytes from a file and stores them in a buffer.
 * Returns the number of bytes read or -1 in case of error.
 */
int readFS(int fileDescriptor, void *buffer, int numBytes);

/*
 * Reads number of bytes from a buffer and writes them in a file.
 * Returns the number of bytes written, 0 in case of end of file or -1 in case
 * of error.
 */
int writeFS(int fileDescriptor, void *buffer, int numBytes);

/*
 * Repositions the pointer of a file. A greater offset than the current size, or
 * an offset lower than 0 are considered errors.
 * Returns new position or -1 in case of error.
 */
int lseekFS(int fileDescriptor, long offset, int whence);

/**********************/
/* Version management */
/**********************/

/*
 * Tags a file with the given tag name. Returns 0 if the operation is
 * successful, 1 if the file already had that tag or -1 in case of error.
 */
int tagFS(char *fileName, char *tagName);

/*
 * Removes a tag from a file. Returns 0 if the operation is successful, 1 if
 * the tag wasn't associated to the file or -1 in case of error.
 */
int untagFS(char *fileName, char *tagName);

/*
 * Looks for all files tagged with the tag tagName, and stores them in the list
 * passed as a parameter. Returns the number of tagged files found or -1 in
 * case of error.
 */
int listFS(char *tagName, char **files);
#endif
