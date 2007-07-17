/*
 * output_scr.h header file for output_scr.c
 */

#ifndef ADIKT_OUTPUTSCR_H
#define ADIKT_OUTPUTSCR_H

extern void set_cursor_visibility(short val);
extern int get_screen_rows();
extern int get_screen_cols();
extern void set_cursor_pos(int row,int col);
extern void screen_setcolor(int idx);
extern void screen_printchr(char dst);
extern void screen_printf(char *format, ...);
extern void screen_printf_toeol(char *format, ...);
extern void screen_refresh();
extern void screen_init(void);
extern void screen_done(void);

#endif // ADIKT_OUTPUTSCR_H
