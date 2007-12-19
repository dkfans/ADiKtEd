/*
 * var_utils.h header file for var_utils.c
 */

#ifndef ADIKT_VARUTILS_H
#define ADIKT_VARUTILS_H

void init_messages();
void free_messages();

void strip_crlf(char *string_in);

void message_error(const char *format, ...);
void message_info(const char *format, ...);
void message_info_force(const char *format, ...);
short message_is_empty();
void message_release();
char *message_get();

void popup_show(const char *title,const char *format, ...);

int arr_ushort_pos(const unsigned short *arr,unsigned short slab_type,int array_count);

short set_msglog_fname(char *fname);
short format_map_fname(char *fname, char *usrinput);

int rnd(int range);

void die(const char *format, ...);
void done(void);

#endif // ADIKT_VARUTILS_H
