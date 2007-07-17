/*
 * graffiti.h header file for graffiti.c
 */

#ifndef ADIKT_GRAFFITI_H
#define ADIKT_GRAFFITI_H

extern int fontheight;

// Imports from graffiti.c
extern int add_graffiti (int x, int y, char *string, int d);
extern void draw_graffiti (void);
extern void read_graffiti (void);
extern void delete_graffiti (int n);
extern char *get_graffiti (int n);
extern int is_graffiti (int x, int y);
extern void free_graffiti (void);

// Import the font from graffiti_font.c
extern unsigned char *chars[];
extern int fontheight;

#endif // ADIKT_GRAFFITI_H
