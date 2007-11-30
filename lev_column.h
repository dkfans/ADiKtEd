/*
 * lev_column.h
 *
 * Header file for lev_column.c.
 *
 */

#ifndef ADIKT_LEVCOLMN_H
#define ADIKT_LEVCOLMN_H

struct LEVEL;
struct COLUMN_REC;

extern int default_clm;

void set_clm_ent_idx(struct LEVEL *lvl, int num, unsigned int use, int permanent,
        int lintel, int height, unsigned int solid, int base, int orientation,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7);
void set_clm(struct LEVEL *lvl, int num, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7);

int column_find_or_create(struct LEVEL *lvl,struct COLUMN_REC *clm_rec);
int column_get_free_index(struct LEVEL *lvl);
short columns_verify(struct LEVEL *lvl, char *err_msg);

int get_dat_subtile(struct LEVEL *lvl, int cx, int cy);
void set_dat_subtile(struct LEVEL *lvl, int cx, int cy, int d);
void set_dat (struct LEVEL *lvl,int x, int y, int tl, int tm, int tr,
        int ml, int mm, int mr, int bl, int bm, int br);
void set_dat_unif (struct LEVEL *lvl, int x, int y, int d);

void update_datclm_for_slab(struct LEVEL *lvl, int x, int y);
void update_clm_use_counters(struct LEVEL *lvl);
void get_slab_surround(unsigned char *surr_slb,unsigned char *surr_own,int x, int y);


//OLD functions
void create_default_clm(void);
void finish_clm (void);
void set_room_wall (int num, unsigned int use, int base,
          int c1, int c2, int c3, int c4, int stat);
void set_corner_unif (int x, int y, int normal, int border);
void set_corner (int x, int y, int normal, int b1, int b2, 
                  int b3, int b4, int b5, int b6, int b7, int b8);

void set_pillar_dat_entry(int x, int y, int pillar);
void update_dat_entry(struct LEVEL *lvl, int x, int y);


#endif // ADIKT_LEVCOLMN_H
