/******************************************************************************/
// obj_actnpts.h - Dungeon Keeper Tools.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  04 Dec 2007

// Purpose:
//   Header file. Defines exported routines from obj_actnpts.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_OBJACTNPTS_H
#define ADIKT_OBJACTNPTS_H

struct LEVEL;

#include "globals.h"

DLLIMPORT unsigned char get_actnpt_subtile_x(unsigned char *actnpt);
DLLIMPORT short set_actnpt_subtile_x(unsigned char *actnpt,unsigned char pos_x);
DLLIMPORT unsigned char get_actnpt_subtile_y(unsigned char *actnpt);
DLLIMPORT short set_actnpt_subtile_y(unsigned char *actnpt,unsigned char pos_y);
DLLIMPORT unsigned char get_actnpt_range_subtile(unsigned char *actnpt);
DLLIMPORT short set_actnpt_range_subtile(unsigned char *actnpt,unsigned char rng_tl);
DLLIMPORT short set_actnpt_subtile(unsigned char *actnpt,unsigned char pos_x,unsigned char pos_y);
DLLIMPORT unsigned char get_actnpt_subtpos_x(unsigned char *actnpt);
DLLIMPORT short set_actnpt_subtpos_x(unsigned char *actnpt,unsigned char pos_x);
DLLIMPORT unsigned char get_actnpt_subtpos_y(unsigned char *actnpt);
DLLIMPORT short set_actnpt_subtpos_y(unsigned char *actnpt,unsigned char pos_y);
DLLIMPORT unsigned char get_actnpt_range_subtpos(unsigned char *actnpt);
DLLIMPORT short set_actnpt_range_subtpos(unsigned char *actnpt,unsigned char rng_st);
DLLIMPORT unsigned int get_actnpt_range_adv(unsigned char *actnpt);
DLLIMPORT short set_actnpt_subtpos(unsigned char *actnpt,unsigned char pos_x,unsigned char pos_y);
DLLIMPORT unsigned short get_actnpt_number(unsigned char *actnpt);
DLLIMPORT short set_actnpt_number(unsigned char *actnpt,unsigned short apt_num);

DLLIMPORT unsigned char *create_actnpt(struct LEVEL *lvl, unsigned int sx, unsigned int sy);
DLLIMPORT unsigned char *create_actnpt_copy(unsigned int sx, unsigned int sy,unsigned char *src);
DLLIMPORT unsigned short get_free_actnpt_number(const struct LEVEL *lvl);
DLLIMPORT unsigned short get_free_actnpt_number_next(const struct LEVEL *lvl,const unsigned short start);
DLLIMPORT unsigned short get_free_actnpt_number_prev(const struct LEVEL *lvl,const unsigned short start);
DLLIMPORT short create_actnpt_number_used_arr(const struct LEVEL *lvl,unsigned char **used,unsigned int *used_size);

DLLIMPORT unsigned char get_stlight_subtile_x(unsigned char *stlight);
DLLIMPORT short set_stlight_subtile_x(unsigned char *stlight,unsigned char pos_x);
DLLIMPORT unsigned char get_stlight_subtile_y(unsigned char *stlight);
DLLIMPORT short set_stlight_subtile_y(unsigned char *stlight,unsigned char pos_y);
DLLIMPORT short set_stlight_subtile(unsigned char *stlight,unsigned char pos_x,unsigned char pos_y);
DLLIMPORT unsigned char get_stlight_subtile_h(unsigned char *stlight);
DLLIMPORT short set_stlight_subtile_h(unsigned char *stlight,unsigned char pos_h);
DLLIMPORT unsigned char get_stlight_subtpos_x(unsigned char *stlight);
DLLIMPORT short set_stlight_subtpos_x(unsigned char *stlight,unsigned char pos_x);
DLLIMPORT unsigned char get_stlight_subtpos_y(unsigned char *stlight);
DLLIMPORT short set_stlight_subtpos_y(unsigned char *stlight,unsigned char pos_y);
DLLIMPORT short set_stlight_subtpos(unsigned char *stlight,unsigned char pos_x,unsigned char pos_y);
DLLIMPORT unsigned char get_stlight_subtpos_h(unsigned char *stlight);
DLLIMPORT short set_stlight_subtpos_h(unsigned char *stlight,unsigned char pos_h);
DLLIMPORT unsigned char get_stlight_range_subtile(unsigned char *stlight);
DLLIMPORT short set_stlight_range_subtile(unsigned char *stlight,unsigned char rng_tl);
DLLIMPORT unsigned char get_stlight_range_subtpos(unsigned char *stlight);
DLLIMPORT short set_stlight_range_subtpos(unsigned char *stlight,unsigned char rng_st);
DLLIMPORT unsigned int get_stlight_range_adv(unsigned char *stlight);
DLLIMPORT unsigned char get_stlight_intensivity(unsigned char *stlight);
DLLIMPORT short set_stlight_intensivity(unsigned char *stlight,unsigned char intens);

DLLIMPORT unsigned char *create_stlight(unsigned int sx, unsigned int sy);
DLLIMPORT unsigned char *create_stlight_copy(unsigned int sx, unsigned int sy,unsigned char *src);

#endif // ADIKT_OBJACTNPTS_H
