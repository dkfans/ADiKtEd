/*
 * scr_slab.h header file for scr_slab.c
 */

#ifndef ADIKT_SCRSLAB_H
#define ADIKT_SCRSLAB_H

// Variables

struct LEVEL;

typedef struct {
    unsigned char *placekeys;
    int placenkeys;
  } MDSLAB_DATA;

extern MDSLAB_DATA *mdslab;

//Slab mode keys export
extern char *slbkey;

//Functions - init and free
short init_mdslab(void);
void free_mdslab(void);

//Functions - start and stop
short start_mdslab(struct LEVEL *lvl);
void end_mdslab(void);

//Functions - actions and screen
void actions_mdslab(int key);
void draw_mdslab();

//Functions - lower level
void init_mdslab_keys();
void free_mdslab_keys();

//Functions - internal
void slbposcheck(void);
void change_ownership(unsigned char purchaser);
void slb_place_room(unsigned char room);
void slb_place_graffiti(struct LEVEL *lvl,int tx, int ty,unsigned short orient);
void slb_next_graffiti_orient(struct LEVEL *lvl,int graf_idx);

#endif // ADIKT_SCRSLAB_H
