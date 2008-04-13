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

unsigned char get_actnpt_subtile_x(unsigned char *actnpt);
short set_actnpt_subtile_x(unsigned char *actnpt,unsigned char pos_x);
unsigned char get_actnpt_subtile_y(unsigned char *actnpt);
short set_actnpt_subtile_y(unsigned char *actnpt,unsigned char pos_y);
unsigned char get_actnpt_range_subtile(unsigned char *actnpt);
short set_actnpt_range_subtile(unsigned char *actnpt,unsigned char rng_tl);
short set_actnpt_tilepos(unsigned char *actnpt,unsigned char pos_x,unsigned char pos_y);
unsigned char get_actnpt_subtpos_x(unsigned char *actnpt);
short set_actnpt_subtpos_x(unsigned char *actnpt,unsigned char pos_x);
unsigned char get_actnpt_subtpos_y(unsigned char *actnpt);
short set_actnpt_subtpos_y(unsigned char *actnpt,unsigned char pos_y);
unsigned char get_actnpt_range_subtpos(unsigned char *actnpt);
short set_actnpt_range_subtpos(unsigned char *actnpt,unsigned char rng_st);
unsigned int get_actnpt_range_adv(unsigned char *actnpt);
short set_actnpt_subtpos(unsigned char *actnpt,unsigned char pos_x,unsigned char pos_y);
unsigned short get_actnpt_number(unsigned char *actnpt);
short set_actnpt_number(unsigned char *actnpt,unsigned short apt_num);

unsigned char *create_actnpt(struct LEVEL *lvl, unsigned int sx, unsigned int sy);
unsigned char *create_actnpt_copy(unsigned int sx, unsigned int sy,unsigned char *src);
unsigned short get_free_actnpt_number(const struct LEVEL *lvl);
unsigned short get_free_actnpt_number_next(const struct LEVEL *lvl,const unsigned short start);
unsigned short get_free_actnpt_number_prev(const struct LEVEL *lvl,const unsigned short start);
short create_actnpt_number_used_arr(const struct LEVEL *lvl,unsigned char **used,unsigned int *used_size);

unsigned char get_stlight_tilepos_x(unsigned char *stlight);
short set_stlight_tilepos_x(unsigned char *stlight,unsigned char pos_x);
unsigned char get_stlight_tilepos_y(unsigned char *stlight);
short set_stlight_tilepos_y(unsigned char *stlight,unsigned char pos_y);
short set_stlight_tilepos(unsigned char *stlight,unsigned char pos_x,unsigned char pos_y);
unsigned char get_stlight_tilepos_h(unsigned char *stlight);
short set_stlight_tilepos_h(unsigned char *stlight,unsigned char pos_h);
unsigned char get_stlight_subtpos_x(unsigned char *stlight);
short set_stlight_subtpos_x(unsigned char *stlight,unsigned char pos_x);
unsigned char get_stlight_subtpos_y(unsigned char *stlight);
short set_stlight_subtpos_y(unsigned char *stlight,unsigned char pos_y);
short set_stlight_subtpos(unsigned char *stlight,unsigned char pos_x,unsigned char pos_y);
unsigned char get_stlight_subtpos_h(unsigned char *stlight);
short set_stlight_subtpos_h(unsigned char *stlight,unsigned char pos_h);
unsigned char get_stlight_range_subtile(unsigned char *stlight);
short set_stlight_range_subtile(unsigned char *stlight,unsigned char rng_tl);
unsigned char get_stlight_range_subtpos(unsigned char *stlight);
short set_stlight_range_subtpos(unsigned char *stlight,unsigned char rng_st);
unsigned int get_stlight_range_adv(unsigned char *stlight);
unsigned char get_stlight_intensivity(unsigned char *stlight);
short set_stlight_intensivity(unsigned char *stlight,unsigned char intens);

unsigned char *create_stlight(unsigned int sx, unsigned int sy);
unsigned char *create_stlight_copy(unsigned int sx, unsigned int sy,unsigned char *src);

#endif // ADIKT_OBJACTNPTS_H
