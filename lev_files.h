/*
 * lev_files.h header file for lev_files.c
 */

#ifndef ADIKT_LEVFILES_H
#define ADIKT_LEVFILES_H

#include "lev_data.h"

#define ERR_NONE         0x000
#define ERR_FILE_NFOUND  0x001
#define ERR_FILE_TOOSMLL 0x002
#define ERR_FILE_BADDATA 0x003
#define ERR_INTERNAL     0x004

short save_map(struct LEVEL *lvl);

short load_map(struct LEVEL *lvl);

unsigned short script_load_and_execute(struct LEVEL *lvl,char *fname,char *err_msg);
short write_text_file(char **lines,int lines_count,char *fname);

#endif // ADIKT_LEVFILES_H
