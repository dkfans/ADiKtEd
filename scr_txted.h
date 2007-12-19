/*
 * scr_txted.h header file for scr_txted.c
 */

#ifndef ADIKT_SCRTXTED_H
#define ADIKT_SCRTXTED_H

// Variables

struct LEVEL;

typedef struct {
    int prevmode;
    int y;
    int rows;
    unsigned char **text;
  } TXTED_DATA;

extern TXTED_DATA *editor;

//Functions - init and free
short init_scrpt(void);
void free_scrpt(void);

//Functions - start and stop
short start_scrpt(struct LEVEL *lvl);
void end_scrpt(void);

//Functions - actions and screen
void actions_scrpt(int key);
void draw_scrpt(void);

//Functions - lower level

//Functions - internal

#endif // ADIKT_SCRTXTED_H
