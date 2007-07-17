/*
 * scr_thing.h header file for scr_thing.c
 */

#ifndef ADIKT_SCRTHING_H
#define ADIKT_SCRTHING_H

extern void draw_tng(void);

extern void display_tngdat(void);
extern char get_thing_char (int x, int y);
extern void display_thing (unsigned char *thing, int x, int y);
extern void display_action_point (unsigned char *point, int x, int y);

#endif // ADIKT_SCRTHING_H
