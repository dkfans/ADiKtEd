/******************************************************************************/
/** @file graffiti.c
 * Wall graffiti support for levels.
 * @par Purpose:
 *     Module for handling wall graffities in levels.
 * @par Comment:
 *     None.
 * @author   Jon Skeet, Tomasz Lis
 * @date     14 Oct 1997 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include <string.h>
#include "graffiti.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_column_def.h"
#include "obj_things.h"
#include "obj_slabs.h"
#include "obj_column.h"
#include "msg_log.h"
#include "lev_column.h"

/**
 * Frees the whole graffiti structure.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short level_free_graffiti(struct LEVEL *lvl)
{
    int i;
    for (i=lvl->graffiti_count-1; i>=0 ; i--)
    {
      free (lvl->graffiti[i]->text);
      free (lvl->graffiti[i]);
    }
    if (lvl->graffiti_count>0)
      free(lvl->graffiti);
    return ERR_NONE;
}

/**
 * Searches for graffiti at given tile and returns its index.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map tile coordinates.
 * @return Returns index of first graffiti at given tile, or -1 if not found.
 */
int graffiti_idx(struct LEVEL *lvl, int tx, int ty)
{
    return graffiti_idx_next(lvl, tx, ty,-1);
}

/**
 * Searches for next graffiti at given tile and returns its index.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx Map tile coordinate, in range 0-MAP_MAXINDEX_X.
 * @param ty Map tile coordinate, in range 0-MAP_MAXINDEX_Y.
 * @param prev_idx index if the graffiti previously found.
 *     To get first graffiti, prev_idx must be -1.
 * @return Returns index of next graffiti at given tile,
 *     the one after prev_idx; or -1 if not found.
 */
int graffiti_idx_next(struct LEVEL *lvl, int tx, int ty, int prev_idx)
{
    if (prev_idx < -1) return -1;
    int i;
    struct DK_GRAFFITI *graf;
    for (i=prev_idx+1; i < lvl->graffiti_count; i++)
    {
      graf = lvl->graffiti[i];
      if ((tx>=graf->tile.x) && (tx<=graf->fin_tile.x) && (ty>=graf->tile.y) && (ty<=graf->fin_tile.y))
          return i;
      /* This makes empty/wrong graffitis visible */
      if ((ty==graf->tile.y) && (tx==graf->tile.x))
          return i;
    }
    return -1;
}

/**
 * Gets text message of graffiti with given index.
 * @param lvl Pointer to the LEVEL structure.
 * @param num Graffiti index.
 * @return Returns graffiti message string,
 *     or empty string ("") on error.
 */
char *get_graffiti_text(struct LEVEL *lvl,unsigned int num)
{
    if ((num>=lvl->graffiti_count))
      return "";
    return lvl->graffiti[num]->text;
}

/**
 * Deletes graffiti with given index.
 * The graffiti is removed from structure, and its memory is freed.
 * Level graphics is not updated by this function.
 * @param lvl Pointer to the LEVEL structure.
 * @param num Graffiti index.
 */
void graffiti_del(struct LEVEL *lvl,unsigned int num)
{
    int i;
    if ((lvl==NULL)||(num>=lvl->graffiti_count))
      return;
    struct DK_GRAFFITI *graf;
    graf=lvl->graffiti[num];
    if (graf!=NULL)
    {
      free(graf->text);
      free(graf);
    }
    int graff_max_idx=lvl->graffiti_count-1;
    for (i=num; i < graff_max_idx; i++)
    {
      lvl->graffiti[i]=lvl->graffiti[i+1];
    }
    /*Decrease the graffiti_count by one */
    lvl->graffiti_count=graff_max_idx;
    /*Decrease amount of allocated memory, or free the block */
    lvl->graffiti = (struct DK_GRAFFITI **)realloc(lvl->graffiti,
             (graff_max_idx)*sizeof(struct DK_GRAFFITI *));
}

/**
 * Creates a new graffiti and fills its all properties.
 * The graffiti is not added to LEVEL structure.
 * @param tx Map tile coordinate, in range 0-MAP_MAXINDEX_X.
 * @param ty Map tile coordinate, in range 0-MAP_MAXINDEX_Y.
 * @param text The message text which is duplicated into graffiti.
 * @param lvl Pointer to the LEVEL structure.
 * @param orient Graffiti orientation, from GRAFFITI_ORIENT enumeration.
 * @return Returns the new graffiti, or NULL on error.
 */
struct DK_GRAFFITI *create_graffiti(int tx, int ty, char *text, const struct LEVEL *lvl, int orient)
{
    tx%=lvl->tlsize.x;
    ty%=lvl->tlsize.y;
    if (text==NULL) return NULL;
    struct DK_GRAFFITI *graf;
    /*Filling graffiti structure */
    graf = (struct DK_GRAFFITI *)malloc(sizeof(struct DK_GRAFFITI));
    if (graf==NULL)
    {
        message_error("Cannot alloc memory for graffiti item");
        return NULL;
    }
    graf->tile.x=tx;
    graf->tile.y=ty;
    graf->font=GRAFF_FONT_ADICLSSC;
    graf->cube=0x0184;
    graf->text = strdup(text);
    set_graffiti_orientation(graf,lvl,orient);
    return graf;
}

/**
 * Adds graffiti object to level data, without filling the graffiti
 * nor updating slabs.
 * @param lvl Pointer to the LEVEL structure.
 * @param graf Pointer to the DK_GRAFFITI structure to add.
 * @return Returns the index at which graffiti is added, or -1 on error.
 */
int graffiti_add_obj(struct LEVEL *lvl,struct DK_GRAFFITI *graf)
{
    if ((lvl==NULL)||(graf==NULL)) return -1;

    int graf_idx=lvl->graffiti_count;
    lvl->graffiti = (struct DK_GRAFFITI **)realloc (lvl->graffiti,
                     (graf_idx+1)*sizeof(struct DK_GRAFFITI *));
    if (lvl->graffiti==NULL)
    {
        message_error("Cannot alloc memory for graffiti array");
        return -1;
    }
    lvl->graffiti[graf_idx]=graf;
    lvl->graffiti_count=graf_idx+1;
    return graf_idx;
}

/**
 * Sets orientation of given graffiti, updating its dimensions and height.
 * Graffiti don't have to be in the LEVEL structure, but must have
 * tx,ty,font and text properties set.
 * @param graf Pointer to the DK_GRAFFITI structure to update.
 * @param lvl Pointer to the LEVEL structure.
 * @param orient New graffiti orientation, from GRAFFITI_ORIENT enumeration.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short set_graffiti_orientation(struct DK_GRAFFITI *graf,const struct LEVEL *lvl,unsigned short orient)
{
    if ((graf==NULL)||(graf->text==NULL)) return false;
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    unsigned short font=graf->font;
    int i;
    int l;
    l = strlen(graf->text);
    /*Computing graffiti length in subtiles */
    int subtl_len=0;
    for (i=0; i<l; i++)
    {
      /*The zero index in every chars[] entry is number of column */
      /*needed for this character. The "+1" is required */
      /* to make space between letters */
      subtl_len+=get_font_char(font,graf->text[i])[0]+1;
      int graf_end_subtl_x,graf_end_subtl_y;
      switch (orient)
      {
      case ORIENT_NS:
      case ORIENT_SN:
          graf_end_subtl_x=graf->tile.x*MAP_SUBNUM_X;
          graf_end_subtl_y=graf->tile.y*MAP_SUBNUM_Y+subtl_len;
          break;
      case ORIENT_WE:
      case ORIENT_EW:
          graf_end_subtl_x=graf->tile.x*MAP_SUBNUM_X+subtl_len;
          graf_end_subtl_y=graf->tile.y*MAP_SUBNUM_Y;
          break;
      case ORIENT_TNS:
      case ORIENT_TSN:
          graf_end_subtl_x=graf->tile.x*MAP_SUBNUM_X;
          graf_end_subtl_y=graf->tile.y*MAP_SUBNUM_Y+subtl_len;
          break;
      case ORIENT_TWE:
      case ORIENT_TEW:
          graf_end_subtl_x=graf->tile.x*MAP_SUBNUM_X+subtl_len;
          graf_end_subtl_y=graf->tile.y*MAP_SUBNUM_Y;
          break;
      default:
          graf_end_subtl_x=graf->tile.x*MAP_SUBNUM_X;
          graf_end_subtl_y=graf->tile.y*MAP_SUBNUM_Y;
          break;
      }
      /*If we've exceeded map space - truncate the graffiti displayed */
      /*(but keep the text in real length) */
      if ((graf_end_subtl_x > arr_entries_x)||(graf_end_subtl_y > arr_entries_y))
      {
        l=i;
        break;
      }
    }
    /* Remove space after last character */
    subtl_len--;
    /*We will need string length in tiles */
    int tiles_len=(subtl_len/MAP_SUBNUM_X) + ((subtl_len%MAP_SUBNUM_X)>0);
    if (tiles_len<1) tiles_len=1;
    /* Getting graffiti height - in subtiles and tiles */
    int txt_height=get_graffiti_cube_height(graf->font,graf->text);
    int txt_tile_height=(txt_height/MAP_SUBNUM_X)+((txt_height%MAP_SUBNUM_X)>0);
    /*Now we can set the graffiti size and orientation */
    graf->orient=orient;
    int graf_h=6-txt_height;
    if ((graf_h<0)||(graf_h>7))
      graf_h=0;
    switch (orient)
    {
      case ORIENT_NS:
      case ORIENT_SN:
           graf->fin_tile.x = graf->tile.x;
           graf->fin_tile.y = graf->tile.y+tiles_len-1;
           graf->height=graf_h;
           break;
      case ORIENT_WE:
      case ORIENT_EW:
           graf->fin_tile.x = graf->tile.x+tiles_len-1;
           graf->fin_tile.y = graf->tile.y;
           graf->height=graf_h;
           break;
      case ORIENT_TNS:
      case ORIENT_TSN:
           graf->fin_tile.x = graf->tile.x+txt_tile_height-1;
           graf->fin_tile.y = graf->tile.y+tiles_len-1;
           if (slab_is_tall(get_tile_slab(lvl,graf->tile.x,graf->tile.y)))
             graf->height=4;
           else
             graf->height=0;
           break;
      case ORIENT_TWE:
      case ORIENT_TEW:
           graf->fin_tile.x = graf->tile.x+tiles_len-1;
           graf->fin_tile.y = graf->tile.y+txt_tile_height-1;
           if (slab_is_tall(get_tile_slab(lvl,graf->tile.x,graf->tile.y)))
             graf->height=4;
           else
             graf->height=0;
           break;
      default:
           graf->fin_tile.x = graf->tile.x+tiles_len-1;
           graf->fin_tile.y = graf->tile.y+tiles_len-1;
           graf->height=graf_h;
           break;
    }
    return ERR_NONE;
}

/**
 * Sets new height to the graffiti. Makes sure the parameter will be
 * in appropiate range.
 * @param graf Pointer to the DK_GRAFFITI structure to update.
 * @param height New graffiti height.
 * @return Returns the new height set for graffiti.
 */
int set_graffiti_height(struct DK_GRAFFITI *graf,int height)
{
    if (graf==NULL) return 0;
    int height_max;
    switch (graf->orient)
    {
      case ORIENT_NS:
      case ORIENT_SN:
      case ORIENT_WE:
      case ORIENT_EW:
           height_max=7-get_graffiti_cube_height(graf->font,graf->text);
           break;
      case ORIENT_TNS:
      case ORIENT_TSN:
      case ORIENT_TWE:
      case ORIENT_TEW:
      default:
           height_max=7;
           break;
    }
    if (height<0) height=0;
    if (height>height_max) height=height_max;
    graf->height=height;
    return height;
}

/**
 * Creates and adds graffiti to the level.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx Map tile coordinate, in range 0-MAP_MAXINDEX_X.
 * @param ty Map tile coordinate, in range 0-MAP_MAXINDEX_Y.
 * @param height Graffiti height.
 * @param text The graffiti message text.
 * @param font Graffiti font, from GRAFFITI_FONT enumeration.
 * @param orient Graffiti orientation, from GRAFFITI_ORIENT enumeration.
 * @param cube Index of the cube used to draw graffiti text.
 * @return Returns the index at which graffiti is added, or -1 on error.
 */
int graffiti_add(struct LEVEL *lvl,int tx, int ty,int height, char *text,int font,
      unsigned short orient,unsigned short cube)
{
    if ((lvl==NULL)||(text==NULL)||(strlen(text)<1)) return -1;
    struct DK_GRAFFITI *graf=create_graffiti(tx,ty,text,lvl,orient);
    if (graf==NULL) return false;
    graf->font=font;
    graf->height=height;
    graf->cube=cube;
    int graf_idx=graffiti_add_obj(lvl,graf);
    if (graf_idx<0)
    {
      free(graf->text);
      free(graf);
    }
    return graf_idx;
}

/**
 * Updates CLM entries to make the graffiti visible.
 * @param lvl Pointer to the LEVEL structure.
 * @param graf_idx Graffiti index.
 */
void graffiti_update_columns(struct LEVEL *lvl,int graf_idx)
{
    if (graf_idx<0) return;
    struct DK_GRAFFITI *graf;
    graf=get_graffiti(lvl, graf_idx);
    if (graf==NULL) return;
    update_datclm_for_square(lvl,graf->tile.x,graf->fin_tile.x,graf->tile.y,graf->fin_tile.y);
}

/**
 * Updates CLM entries, removing graffiti from them.
 * This makes the graffiti invisuble and ready to be deleted.
 * @param lvl Pointer to the LEVEL structure.
 * @param graf_idx Graffiti index.
 */
void graffiti_clear_from_columns(struct LEVEL *lvl,int graf_idx)
{
    if (graf_idx<0) return;
    struct DK_GRAFFITI *graf;
    graf=get_graffiti(lvl, graf_idx);
    if (graf==NULL) return;
    int tx,fin_tx,ty,fin_ty;
    /*Setting graffiti coords to off-screen */
    tx=graf->tile.x;graf->tile.x=lvl->tlsize.x;
    fin_tx=graf->fin_tile.x;graf->fin_tile.x=lvl->tlsize.x;
    ty=graf->tile.y;graf->tile.y=lvl->tlsize.y;
    fin_ty=graf->fin_tile.y;graf->fin_tile.y=lvl->tlsize.y;
    /*Updating */
    update_datclm_for_square(lvl,tx,fin_tx,ty,fin_ty);
    /*Setting the coords back */
    graf->tile.x=tx;
    graf->fin_tile.x=fin_tx;
    graf->tile.y=ty;
    graf->fin_tile.y=fin_ty;
}

/**
 * Gets amount of graffitties in the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns graffities count for the level.
 */
unsigned int get_graffiti_count(struct LEVEL *lvl)
{
    if (lvl==NULL) return 0;
    return lvl->graffiti_count;
}

/**
 * Gets the graffiti structure for given graffiti index.
 * @param lvl Pointer to the LEVEL structure.
 * @param graf_idx Graffiti index.
 * @return Returns DK_GRAFFITI structure pointer, NULL on error.
 */
struct DK_GRAFFITI *get_graffiti(struct LEVEL *lvl, int graf_idx)
{
    if ((graf_idx<0)||(graf_idx>=lvl->graffiti_count)) return NULL;
    return lvl->graffiti[graf_idx];
}

/**
 * Computes graffiti text length in subtiles, for given font.
 * @param font Graffiti font, from GRAFFITI_FONT enumeration.
 * @param text The graffiti message text.
 * @return Returns length of the graffiti, in subtiles.
 */
int compute_graffiti_subtl_length(unsigned short font,char *text)
{
    if (text==NULL) return 0;
    int l = strlen(text);
    if (l<1) return 0;
    switch (font)
    {
    case GRAFF_FONT_ADICLSSC:
    case GRAFF_FONT_ADISIZE8:
         break;
    case GRAFF_FONT_NONE:
    default:
        return 0;
    }
    /*Counting graffiti length in subtiles */
    int i;
    int subtl_len=0;
    for (i=0; i < l; i++)
    {
        /*The zero index in every chars[] entry is number of column */
        /*needed for this character. The "+1" is required */
        /* to make space between letters */
        subtl_len+=get_font_char(font,text[i])[0]+1;
    }
    /* Remove space after last char */
    subtl_len--;
    return subtl_len;
}

/**
 * Returns graffiti text height in subtiles, for given font.
 * @param font Graffiti font, from GRAFFITI_FONT enumeration.
 * @param text The graffiti message text.
 * @return Returns graffiti height, in cubes (or subtiles).
 */
int get_graffiti_cube_height(unsigned short font, __attribute__((unused)) char *text)
{
    /*highly simplified - for now... */
    /*TODO: compute height of every char in text, then select the largest. */
  switch (font)
  {
  case GRAFF_FONT_ADICLSSC:
    return font_adiclssc_height;
  case GRAFF_FONT_ADISIZE8:
    return font_adisize8_height;
  default:
    return 0;
  }
}

/**
 * Returns graffiti character for given font.
 * @param font Graffiti font, from GRAFFITI_FONT enumeration.
 * @param chr The text character.
 * @return Returns font character to use for making graffiti visible.
 */
const unsigned char *get_font_char(unsigned short font,char chr)
{
  switch (font)
  {
  case GRAFF_FONT_ADICLSSC:
    return font_adiclssc_chars[(unsigned char)chr];
  case GRAFF_FONT_ADISIZE8:
    return font_adisize8_chars[(unsigned char)chr];
  default:
    return font_any_chnull;
  }
}

/**
 * Draws graffiti on given columns array.
 * @param clm_recs Comumn entries array.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx Map tile coordinate, in range 0-MAP_MAXINDEX_X.
 * @param ty Map tile coordinate, in range 0-MAP_MAXINDEX_Y.
 * @return Returns num of changed entries.
 */
int place_graffiti_on_slab(struct COLUMN_REC *clm_recs[9],struct LEVEL *lvl, int tx, int ty)
{
    static const int dy[4][3]={{0,1,2}, {2,2,2}, {2,1,0}, {0,0,0}};
    static const int dx[4][3]={{0,0,0}, {0,1,2}, {2,2,2}, {2,1,0}};
    int mod_clms=0;
    int graf_idx=-1;
    while ((graf_idx=graffiti_idx_next(lvl,tx,ty,graf_idx))>=0)
    {
      struct DK_GRAFFITI *graf;
      graf=get_graffiti(lvl,graf_idx);
      if (graf==NULL) continue;
      /*Setting some local variables */
      int i;
      int base_sx=graf->tile.x*MAP_SUBNUM_X;
      int base_sy=graf->tile.y*MAP_SUBNUM_Y;
      /*Counting graffiti length in subtiles */
      int subtl_len=compute_graffiti_subtl_length(graf->font,graf->text);
      /*Starting part of the graffiti */
      int graf_subtl_start;
      /* Get short access to orientation, and make sure it won't exceed */
      /* dx[][]/dy[][] array */
      short orient_top=false;
      switch (graf->orient)
      {
      case ORIENT_TNS:
        orient_top=true;
        /* fall through */
      case ORIENT_NS:
        graf_subtl_start=ty*MAP_SUBNUM_Y-base_sy;
        break;
      case ORIENT_TSN:
        orient_top=true;
        /* fall through */
      case ORIENT_SN:
        graf_subtl_start=(base_sy+subtl_len-1)-ty*MAP_SUBNUM_Y;
        break;
      case ORIENT_TWE:
        orient_top=true;
        /* fall through */
      case ORIENT_WE:
        graf_subtl_start=tx*MAP_SUBNUM_X-base_sx;
        break;
      case ORIENT_TEW:
        orient_top=true;
        /* fall through */
      case ORIENT_EW:
        graf_subtl_start=(base_sx+subtl_len-1)-tx*MAP_SUBNUM_X;
        break;
      default:
        graf_subtl_start=0;
        break;
      }
      if (orient_top)
      {
        int subtl_h=get_graffiti_cube_height(graf->font,graf->text);
        int graf_subtl_h_st;
        switch (graf->orient)
        {
        case ORIENT_TNS: graf_subtl_h_st=tx*MAP_SUBNUM_X-base_sx; break;
        case ORIENT_TSN: graf_subtl_h_st=(base_sx+subtl_h-1)-(tx+1)*MAP_SUBNUM_X; break;
        case ORIENT_TWE: graf_subtl_h_st=(base_sy+subtl_h-1)-(ty+1)*MAP_SUBNUM_Y; break;
        case ORIENT_TEW: graf_subtl_h_st=ty*MAP_SUBNUM_Y-base_sy; break;
        default: graf_subtl_h_st=0; break;
        }
        for (i=0; i<MAP_SUBNUM_X; i++)
        {
          int k;
          int graf_subtl=graf_subtl_start+i;
          for (k=0; k<MAP_SUBNUM_Y; k++)
          {
            int subtl;
            switch (graf->orient)
            {
            case ORIENT_TNS: subtl=i*MAP_SUBNUM_X + k; break;
            case ORIENT_TSN: subtl=(2-i)*MAP_SUBNUM_X + (2-k); break;
            case ORIENT_TWE: subtl=(2-k)*MAP_SUBNUM_X + i; break;
            case ORIENT_TEW: subtl=k*MAP_SUBNUM_X + (2-i); break;
            default: subtl=0; break;
            }
            int graf_subtl_h=graf_subtl_h_st+k;
            short modified=place_graffiti_on_clm_top(clm_recs[subtl],graf->font,
                  graf->height,graf->text,graf_subtl,graf_subtl_h,graf->cube);
            if (modified) mod_clms++;
          }
        }
      } else
      {
        unsigned short or=((graf->orient%4));
        for (i=0; i<MAP_SUBNUM_X; i++)
        {
          int subtl=dy[or][i]*MAP_SUBNUM_X + dx[or][i];
          int graf_subtl=graf_subtl_start+i;
          short modified=place_graffiti_on_column(clm_recs[subtl],graf->font,
                graf->height,graf->text,graf_subtl,graf->cube);
          if (modified) mod_clms++;
        }
      }
    }
    return mod_clms;
}

/**
 * Places graffiti on top of given column.
 * @param clm_rec Comumn entry struct pointer.
 * @param font Graffiti font, from GRAFFITI_FONT enumeration.
 * @param height Graffiti height.
 * @param text The graffiti message text.
 * @param graf_subtl The subtile at which we're placing.
 * @param graf_subtl_h The subtile at which we're placing, on height.
 * @param cube Index of the cube to place as graffiti.
 * @return Returns true on success.
 */
short place_graffiti_on_clm_top(struct COLUMN_REC *clm_rec,unsigned short font,
        unsigned short height,char *text,int graf_subtl,int graf_subtl_h,
        unsigned short cube)
{
    if ((clm_rec==NULL)||(text==NULL)||(strlen(text)<1)) return false;
    int i;
    int l = strlen(text);
    int text_pos=0; /*position of the character to print inside text */
    int clm_pos=0; /*index of the current column in the current character */
    i=0;/*temporary column counter */
    for (text_pos=0; text_pos<l; text_pos++)
    {
        /*The zero index in every chars[] entry is number of column */
        /*needed for this character. */
        int chr_clms_count=get_font_char(font,text[text_pos])[0]+1;
        if ((i<=graf_subtl)&&(i+chr_clms_count>graf_subtl))
        {
            /*clm_pos starts with 1, not with 0 (because chars[][0] is */
            /* the number of columns in a letter) */
            clm_pos=graf_subtl-i+1;
            break;
        }
        i+=chr_clms_count;
    }
    /*Check if we've found the right position */
    if (text_pos>=l) return false;
    const unsigned char *char_data=get_font_char(font,text[text_pos]);
    /* Check if this is empty column (space between letters) */
    if ((clm_pos<=0)||(clm_pos>char_data[0])) return false;
    unsigned char clm_mask=char_data[clm_pos];
    if ((clm_mask>>graf_subtl_h)&0x01)
    {
        if (height<8) clm_rec->c[height]=cube;
    }
    clm_rec->solid=compute_clm_rec_solid(clm_rec);
    clm_rec->height=compute_clm_rec_height(clm_rec);
    return true;
}

/**
 * Places graffiti on side of given column.
 * @param clm_rec Comumn entry struct pointer.
 * @param font Graffiti font, from GRAFFITI_FONT enumeration.
 * @param height Graffiti height.
 * @param text The graffiti message text.
 * @param graf_subtl The subtile at which we're placing.
 * @param cube Index of the cube to place as graffiti.
 * @return Returns true on success.
 */
short place_graffiti_on_column(struct COLUMN_REC *clm_rec,unsigned short font,
        unsigned short height,char *text,int graf_subtl,unsigned short cube)
{
    if ((clm_rec==NULL)||(text==NULL)||(strlen(text)<1)) return false;
    int i;
    int l = strlen(text);
    int text_pos=0; /*position of the character to print inside text */
    int clm_pos=0; /*index of the current column in the current character */
    i=0;/*temporary column counter */
    for (text_pos=0; text_pos<l; text_pos++)
    {
        /*The zero index in every chars[] entry is number of column */
        /*needed for this character. */
        int chr_clms_count=get_font_char(font,text[text_pos])[0]+1;
        if ((i<=graf_subtl)&&(i+chr_clms_count>graf_subtl))
        {
            /*clm_pos starts with 1, not with 0 (because chars[][0] is */
            /* the number of columns in letter) */
            clm_pos=graf_subtl-i+1;
            break;
        }
        i+=chr_clms_count;
    }
    /*Check if we've found the right position */
    if (text_pos>=l) return false;
    const unsigned char *char_data=get_font_char(font,text[text_pos]);
    /* Check if this is empty column (space between letters) */
    if ((clm_pos<=0)||(clm_pos>char_data[0])) return false;
    unsigned char clm_mask=char_data[clm_pos];
    for (i=0;i<8;i++)
    {
        if ((clm_mask>>i)&0x01)
        {
            int idx=i+height;
            if ((idx>=0)&&(idx<8)) clm_rec->c[idx]=cube;
        }
    }
    clm_rec->solid=compute_clm_rec_solid(clm_rec);
    clm_rec->height=compute_clm_rec_height(clm_rec);
    return true;
}
