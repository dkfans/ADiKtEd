/*
 * output_scr.h header file for output_scr.c
 */

#ifndef ADIKT_OUTPUTSCR_H
#define ADIKT_OUTPUTSCR_H

enum borderStyle {bsSingle,bsDouble,bsSolid};

#define PRINT_COLOR_LGREY_ON_BLACK    0
#define PRINT_COLOR_BLACK_ON_LGREY    1
#define PRINT_COLOR_YELLOW_ON_BLUE    2
#define PRINT_COLOR_LRED_ON_BLACK     3
#define PRINT_COLOR_YELLOW_ON_BLACK   4
#define PRINT_COLOR_WHITE_ON_BLACK    5
#define PRINT_COLOR_LMAGENT_ON_BLACK  7
#define PRINT_COLOR_RED_ON_WHITE      9

#define PRINT_COLOR_WHITE_ON_RED      10
#define PRINT_COLOR_WHITE_ON_BLUE     11
#define PRINT_COLOR_WHITE_ON_GREEN    12
#define PRINT_COLOR_WHITE_ON_BROWN    13
#define PRINT_COLOR_WHITE_ON_CYAN     14
#define PRINT_COLOR_GREY_ON_BLACK     16
#define PRINT_COLOR_MAGENT_ON_WHITE   17
#define PRINT_COLOR_BLUE_ON_LGREY     21
#define PRINT_COLOR_GREEN_ON_WHITE    22
#define PRINT_COLOR_YELLOW_ON_LGREY   23
#define PRINT_COLOR_CYAN_ON_WHITE     24
    
void set_cursor_visibility(short val);
int get_screen_rows();
int get_screen_cols();
void set_cursor_pos(int row,int col);
void screen_setcolor(int idx);
void screen_printchr(char dst);
void screen_printf(char *format, ...);
void screen_printf_toeol(char *format, ...);
void screen_draw_hline(int posy,int posx,int length,short border_style);
void screen_draw_vline(int posy,int posx,int length,short border_style);
void screen_draw_window(int posy,int posx,int sizey,int sizex,int border_size,short border_style);
void screen_refresh();
void screen_init(void);
void screen_done(void);


#endif // ADIKT_OUTPUTSCR_H
