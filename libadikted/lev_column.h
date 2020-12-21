/******************************************************************************/
/** @file lev_column.h
 * Level graphics (DAT/CLM) handling module.
 * @par Purpose:
 *     Header file. Defines exported routines from lev_column.c
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     21 Jan 2008 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef ADIKT_LEVCOLMN_H
#define ADIKT_LEVCOLMN_H

//Size of texture/tileset list
#define INF_MAX_INDEX 10

struct LEVEL;
struct COLUMN_REC;
struct DK_CUSTOM_CLM;
struct IPOINT_2D;

#include "globals.h"

void set_clm_ent_idx(struct LEVEL *lvl, int num, unsigned int use, int permanent,
        int lintel, int height, unsigned int solid, int base, int orientation,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7);
DLLIMPORT void set_clm(struct LEVEL *lvl, int num, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7);

DLLIMPORT int column_find_or_create(struct LEVEL *lvl,struct COLUMN_REC *clm_rec);
DLLIMPORT int column_get_free_index(struct LEVEL *lvl);
DLLIMPORT short columns_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt);

DLLIMPORT unsigned int get_dat_subtile(const struct LEVEL *lvl, const unsigned int sx, const unsigned int sy);
DLLIMPORT void set_dat_subtile(struct LEVEL *lvl, int sx, int sy, int d);
void set_dat(struct LEVEL *lvl,int x, int y, int tl, int tm, int tr,
        int ml, int mm, int mr, int bl, int bm, int br);
void set_dat_unif (struct LEVEL *lvl, int x, int y, int d);
DLLIMPORT short find_dat_entry(const struct LEVEL *lvl, int *sx, int *sy, const unsigned int clm_idx);
DLLIMPORT short dat_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt);

DLLIMPORT void update_datclm_for_whole_map(struct LEVEL *lvl);
DLLIMPORT void update_datclm_for_square_radius1(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void update_datclm_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last);
DLLIMPORT void update_datclm_for_slab(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void set_new_datclm_values(struct LEVEL *lvl, int tx, int ty, struct COLUMN_REC *clm_recs[9]);
DLLIMPORT void set_new_datclm_entry(struct LEVEL *lvl, int sx, int sy, struct COLUMN_REC *clm_rec);
DLLIMPORT void update_clm_utilize_counters(struct LEVEL *lvl);
DLLIMPORT void get_slab_surround(unsigned char *surr_slb,unsigned char *surr_own,
    unsigned char **surr_tng,const struct LEVEL *lvl,int tx, int ty);
DLLIMPORT void clm_utilize_dec(struct LEVEL *lvl, int clmidx);
DLLIMPORT void clm_utilize_inc(struct LEVEL *lvl, int clmidx);
DLLIMPORT short clm_entry_is_used(const struct LEVEL *lvl,unsigned int clmidx);
DLLIMPORT short update_dat_last_column(struct LEVEL *lvl, unsigned short slab);

DLLIMPORT void update_tile_wib_entries(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void update_tile_wlb_entry(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void update_tile_flg_entries(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT unsigned short compute_flg_for_tile(unsigned short slab,short corner);

DLLIMPORT unsigned char *get_subtile_column(const struct LEVEL *lvl, int sx, int sy);
DLLIMPORT short get_subtile_column_rec(const struct LEVEL *lvl, struct COLUMN_REC *clm_rec, int sx, int sy);
DLLIMPORT unsigned short get_subtile_column_height(struct LEVEL *lvl, int sx, int sy);
DLLIMPORT unsigned short get_subtile_column_solid(struct LEVEL *lvl, int sx, int sy);

/* Custom columns support; graffiti are also custom columns */
DLLIMPORT short slab_has_custom_columns(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT int update_custom_columns_for_slab(struct COLUMN_REC *clm_recs[9],struct LEVEL *lvl, int tx, int ty);
DLLIMPORT int place_cust_clms_on_slab(struct COLUMN_REC *clm_recs[9],struct LEVEL *lvl, int tx, int ty);
DLLIMPORT short cust_col_add_or_update(struct LEVEL *lvl,int sx,int sy,struct DK_CUSTOM_CLM *ccol);
DLLIMPORT int cust_cols_num_on_tile(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT struct DK_CUSTOM_CLM *get_cust_col(struct LEVEL *lvl, int sx, int sy);
DLLIMPORT struct COLUMN_REC *get_cust_col_rec(struct LEVEL *lvl, int sx, int sy);
DLLIMPORT unsigned short get_cust_col_wib_entry(struct LEVEL *lvl, int sx, int sy);
DLLIMPORT short set_cust_col(struct LEVEL *lvl,int sx,int sy,struct DK_CUSTOM_CLM *ccol);
DLLIMPORT struct DK_CUSTOM_CLM *create_cust_col(void);
DLLIMPORT short cust_col_del(struct LEVEL *lvl, int sx, int sy);
DLLIMPORT int cust_cols_del_for_tile(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT unsigned int get_cust_clm_count(struct LEVEL *lvl);

/*INF file and textures */
DLLIMPORT char *get_texture_fullname(unsigned short inf_type);

/* This one is special - put in separate file */
DLLIMPORT void add_permanent_columns(struct LEVEL *lvl);

#endif /* ADIKT_LEVCOLMN_H */
