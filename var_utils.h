/*
 * var_utils.h header file for var_utils.c
 */

#ifndef ADIKT_VARUTILS_H
#define ADIKT_VARUTILS_H

extern void strip_crlf(char *string_in);

extern void message_error(const char *format, ...);
extern void message_info(const char *format, ...);
extern void message_info_force(const char *format, ...);
extern short message_is_empty();
extern void message_release();
extern char *message_get();

extern short format_map_fname(char *fname, char *usrinput);

extern void die(const char *format, ...);
extern void done(void);

#endif // ADIKT_VARUTILS_H
