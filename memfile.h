/*
 * memfile.h header file
 *
 * header for file.c
 *
 */

#ifndef ADIKT_MEMFILE_H
#define ADIKT_MEMFILE_H

//max. acceptable file size for allreading operations (set to 700MB)
#define MAX_FILE_SIZE 734003200

//first 16 errors are reserved to RNC support
#define MFILE_OK             0
#define MFILE_CANNOT_OPEN  -17
#define MFILE_MALLOC_ERR   -18
#define MFILE_SIZE_ERR     -19
#define MFILE_READ_ERR     -20


struct MEMORY_FILE
{
    unsigned long len;
    unsigned char *content;
    int errcode;
};

struct MEMORY_FILE read_file(char *iname);
char *read_file_error(int errcode);

#endif // ADIKT_MEMFILE_H
