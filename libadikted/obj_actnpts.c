/******************************************************************************/
// obj_actnpts.c - Dungeon Keeper Tools.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  04 Dec 2007

// Purpose:
//   Functions for maintaining single action points - APT entries,
//   and single static lights - LGT entries.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "obj_actnpts.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"

unsigned char get_actnpt_subtile_x(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return actnpt[1];
}

short set_actnpt_subtile_x(unsigned char *actnpt,unsigned char pos_x)
{
    if (actnpt==NULL) return 0;
    actnpt[1]=pos_x;
}

unsigned char get_actnpt_subtile_y(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return actnpt[3];
}

short set_actnpt_subtile_y(unsigned char *actnpt,unsigned char pos_y)
{
    if (actnpt==NULL) return 0;
    actnpt[3]=pos_y;
}

short set_actnpt_subtile(unsigned char *actnpt,unsigned char pos_x,unsigned char pos_y)
{
    if (actnpt==NULL) return 0;
    actnpt[1]=pos_x;
    actnpt[3]=pos_y;
}

unsigned char get_actnpt_range_subtile(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return actnpt[5];
}

short set_actnpt_range_subtile(unsigned char *actnpt,unsigned char rng_tl)
{
    if (actnpt==NULL) return 0;
    actnpt[5]=rng_tl;
}

/*
 * Returns range as subliles*256+subtl_pos
 */
unsigned int get_actnpt_range_adv(unsigned char *actnpt)
{
    return ((unsigned int)get_actnpt_range_subtile(actnpt)<<8)+
        get_actnpt_range_subtpos(actnpt);
}

unsigned char get_actnpt_subtpos_x(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return actnpt[0];
}

short set_actnpt_subtpos_x(unsigned char *actnpt,unsigned char pos_x)
{
    if (actnpt==NULL) return 0;
    actnpt[0]=pos_x;
}

unsigned char get_actnpt_subtpos_y(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return actnpt[2];
}

short set_actnpt_subtpos_y(unsigned char *actnpt,unsigned char pos_y)
{
    if (actnpt==NULL) return 0;
    actnpt[2]=pos_y;
}

short set_actnpt_subtpos(unsigned char *actnpt,unsigned char pos_x,unsigned char pos_y)
{
    if (actnpt==NULL) return 0;
    actnpt[0]=pos_x;
    actnpt[2]=pos_y;
}

unsigned char get_actnpt_range_subtpos(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return actnpt[4];
}

short set_actnpt_range_subtpos(unsigned char *actnpt,unsigned char rng_st)
{
    if (actnpt==NULL) return 0;
    actnpt[4]=rng_st;
}

unsigned short get_actnpt_number(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return (unsigned short)((actnpt[7]<<8)+actnpt[6]);
}

short set_actnpt_number(unsigned char *actnpt,unsigned short apt_num)
{
    if (actnpt==NULL) return 0;
    actnpt[7]=(apt_num>>8);
    actnpt[6]=apt_num&255;
}

/*
 * Creates a new action point, prepared to be placed in given coordinates,
 * but without placing it into LEVEL structure. Gives the action point a proper
 * number and sets range to minimum.
 */
unsigned char *create_actnpt(struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    sx%=arr_entries_x;
    sy%=arr_entries_y;

    unsigned char *actnpt;
    actnpt = (unsigned char *)malloc(SIZEOF_DK_APT_REC);
    if (actnpt==NULL)
    {
        message_error("create_actnpt: Out of memory");
        return NULL;
    }
    int i;
    for (i=0; i < SIZEOF_DK_APT_REC; i++)
      actnpt[i]=0;
    set_actnpt_subtpos(actnpt,0x80,0x80);
    set_actnpt_subtile(actnpt,(unsigned char)sx,(unsigned char)sy);
    set_actnpt_number(actnpt,get_free_actnpt_number(lvl));
    set_actnpt_range_subtile(actnpt,5);
    return actnpt;
}

/*
 * Creates a new action point, prepared to be placed in given coordinates,
 * but without placing it into LEVEL structure. Copies action point number
 * and other properties from given pointer
 */
unsigned char *create_actnpt_copy(unsigned int sx, unsigned int sy,unsigned char *src)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    sx%=arr_entries_x;
    sy%=arr_entries_y;

    unsigned char *actnpt;
    actnpt = (unsigned char *)malloc(SIZEOF_DK_APT_REC);
    if (actnpt==NULL)
    {
        message_error("create_actnpt_copy: Out of memory");
        return NULL;
    }
    memcpy(actnpt,src,SIZEOF_DK_APT_REC);
    set_actnpt_subtpos(actnpt,0x80,0x80);
    set_actnpt_subtile(actnpt,(unsigned char)sx,(unsigned char)sy);
    return actnpt;
}

/*
 * Sweeps action points to find an unused action point number.
 */
unsigned short get_free_actnpt_number(const struct LEVEL *lvl)
{
    get_free_actnpt_number_next(lvl,1);
}

/*
 * Sweeps action points to find an unused action point number,
 * not smaller than given number
 */
unsigned short get_free_actnpt_number_next(const struct LEVEL *lvl,const unsigned short start)
{
    unsigned int used_size=start+16;
    unsigned char *used=NULL;
    if (!create_actnpt_number_used_arr(lvl,&used,&used_size))
        return start;
    int new_num=start;
    while (new_num<used_size)
    {
      if (used[new_num]==0) break;
      new_num++;
    }
    free(used);
    return new_num;
}

/*
 * Sweeps action points to find an unused action point number,
 * not larger than given number
 */
unsigned short get_free_actnpt_number_prev(const struct LEVEL *lvl,const unsigned short start)
{
    unsigned int used_size=start+2;
    unsigned char *used=NULL;
    if (!create_actnpt_number_used_arr(lvl,&used,&used_size))
        return start;
    int new_num=start;
    while (new_num>1)
    {
      if (used[new_num]==0) break;
      new_num--;
    }
    free(used);
    return new_num;
}

/*
 * Creates an array which contains number of uses of each action point number.
 * the array is never smaller than starting value of 'size'; the function may
 * enlarge 'size' and create larger array in some cases.
 * Note: numbering starts at 1, not 0; used[0] is used for 'out of bounds' numbers
 */
short create_actnpt_number_used_arr(const struct LEVEL *lvl,unsigned char **used,unsigned int *used_size)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int k;
    *used_size=max(lvl->apt_total_count+16,*used_size);
    *used=malloc((*used_size)*sizeof(unsigned char));
    if (*used==NULL) return false;
    for (k=0;k<(*used_size);k++)
      (*used)[k]=0;
    int cy, cx;
    for (cy=0; cy<arr_entries_y; cy++)
    {
      for (cx=0; cx<arr_entries_x; cx++)
      {
          int num_subs=get_actnpt_subnums(lvl,cx,cy);
          for (k=0; k<num_subs; k++)
          {
                char *actnpt=get_actnpt(lvl,cx,cy,k);
                unsigned short cnum=get_actnpt_number(actnpt);
                if (cnum<(*used_size))
                  (*used)[cnum]++;
                else
                  (*used)[0]++;
          }
      }
    }
    return true;
}

unsigned char get_stlight_subtile_x(unsigned char *stlight)
{
    if (stlight==NULL) return 0;
    return (unsigned char)stlight[11];
}

short set_stlight_subtile_x(unsigned char *stlight,unsigned char pos_x)
{
    if (stlight==NULL) return 0;
    stlight[11]=pos_x;
}

unsigned char get_stlight_subtile_y(unsigned char *stlight)
{
    if (stlight==NULL) return 0;
    return (unsigned char)stlight[13];
}

short set_stlight_subtile_y(unsigned char *stlight,unsigned char pos_y)
{
    if (stlight==NULL) return 0;
    stlight[13]=pos_y;
}

short set_stlight_subtile(unsigned char *stlight,unsigned char pos_x,unsigned char pos_y)
{
    if (stlight==NULL) return 0;
    stlight[11]=pos_x;
    stlight[13]=pos_y;
}

unsigned char get_stlight_subtile_h(unsigned char *stlight)
{
    if (stlight==NULL) return 0;
    return stlight[15];
}

short set_stlight_subtile_h(unsigned char *stlight,unsigned char pos_h)
{
    if (stlight==NULL) return 0;
    stlight[15]=pos_h;
}

unsigned char get_stlight_subtpos_x(unsigned char *stlight)
{
    if (stlight==NULL) return 0;
    return stlight[10];
}

short set_stlight_subtpos_x(unsigned char *stlight,unsigned char pos_x)
{
    if (stlight==NULL) return 0;
    stlight[10]=pos_x;
}

unsigned char get_stlight_subtpos_y(unsigned char *stlight)
{
    if (stlight==NULL) return 0;
    return stlight[12];
}

short set_stlight_subtpos_y(unsigned char *stlight,unsigned char pos_y)
{
    if (stlight==NULL) return 0;
    stlight[12]=pos_y;
}

short set_stlight_subtpos(unsigned char *stlight,unsigned char pos_x,unsigned char pos_y)
{
    if (stlight==NULL) return 0;
    stlight[10]=pos_x;
    stlight[12]=pos_y;
}

unsigned char get_stlight_subtpos_h(unsigned char *stlight)
{
    if (stlight==NULL) return 0;
    return stlight[14];
}

short set_stlight_subtpos_h(unsigned char *stlight,unsigned char pos_h)
{
    if (stlight==NULL) return 0;
    stlight[14]=pos_h;
}

unsigned char get_stlight_range_subtile(unsigned char *stlight)
{
    if (stlight==NULL) return 0;
    return stlight[1];
}

short set_stlight_range_subtile(unsigned char *stlight,unsigned char rng_tl)
{
    if (stlight==NULL) return 0;
    stlight[1]=rng_tl;
}

unsigned char get_stlight_range_subtpos(unsigned char *stlight)
{
    if (stlight==NULL) return 0;
    return stlight[0];
}

short set_stlight_range_subtpos(unsigned char *stlight,unsigned char rng_st)
{
    if (stlight==NULL) return 0;
    stlight[0]=rng_st;
}

/*
 * Returns range as subliles*256+subtl_pos
 */
unsigned int get_stlight_range_adv(unsigned char *stlight)
{
    return ((unsigned int)get_stlight_range_subtile(stlight)<<8)+
        get_stlight_range_subtpos(stlight);
}

unsigned char get_stlight_intensivity(unsigned char *stlight)
{
    if (stlight==NULL) return 0;
    return stlight[2];
}

short set_stlight_intensivity(unsigned char *stlight,unsigned char intens)
{
    if (stlight==NULL) return 0;
    stlight[2]=intens;
    return 1;
}

/*
 * Creates a new static light, prepared to be placed in given coordinates,
 * but without placing it into LEVEL structure
 */
unsigned char *create_stlight(unsigned int sx, unsigned int sy)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    sx%=arr_entries_x;
    sy%=arr_entries_y;

    unsigned char *stlight;
    stlight = (unsigned char *)malloc(SIZEOF_DK_LGT_REC);
    if (stlight==NULL)
    {
        message_error("create_stlight: Out of memory");
        return NULL;
    }
    int i;
    for (i=0; i < SIZEOF_DK_LGT_REC; i++)
      stlight[i]=0;
    set_stlight_subtpos(stlight,((sx%MAP_SUBNUM_X)*0x40+0x40),((sy%MAP_SUBNUM_Y)*0x40+0x40));
    set_stlight_subtile(stlight,(unsigned char)sx,(unsigned char)sy);
    set_stlight_subtile_h(stlight,5);
    set_stlight_range_subtile(stlight,16);
    set_stlight_range_subtpos(stlight,0);
    set_stlight_intensivity(stlight,32);
    //Other parameters - no idea what they mean, but seem to be constant
    stlight[18]=0x0ff;
    stlight[19]=0x0ff;
    return stlight;
}

/*
 * Creates a new static light, prepared to be placed in given coordinates,
 * but without placing it into LEVEL structure. Copies properties from
 * given pointer.
 */
unsigned char *create_stlight_copy(unsigned int sx, unsigned int sy,unsigned char *src)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    sx%=arr_entries_x;
    sy%=arr_entries_y;

    unsigned char *stlight;
    stlight = (unsigned char *)malloc(SIZEOF_DK_LGT_REC);
    if (stlight==NULL)
    {
        message_error("create_stlight_copy: Out of memory");
        return NULL;
    }
    int i;
    memcpy(stlight,src,SIZEOF_DK_LGT_REC);
    set_stlight_subtpos(stlight,((sx%MAP_SUBNUM_X)*0x40+0x40),((sy%MAP_SUBNUM_Y)*0x40+0x40));
    set_stlight_subtile(stlight,(unsigned char)sx,(unsigned char)sy);
    return stlight;
}
