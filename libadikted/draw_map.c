/******************************************************************************/
/** @file draw_map.c
 * Graphics draw of Dungeon Keeper map.
 * @par Purpose:
 *     Functions needed to create a graphic view of DK1 map using original
 *     textures.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     12 Jan 2008 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include "draw_map.h"

#include <math.h>
#include "globals.h"
#include "memfile.h"
#include "obj_cube.h"
#include "obj_slabs.h"
#include "lev_data.h"
#include "lev_files.h"
#include "lev_column.h"
#include "obj_column.h"
#include "obj_things.h"
#include "xcubtxtr.h"
#include "xtabdat8.h"
#include "xtabjty.h"
#include "msg_log.h"
#include "obj_column_def.h"
#include "obj_actnpts.h"
#include "bulcommn.h"
#include "arr_utils.h"
#include "lev_things.h"

/**
 * Intensified player colors array.
 * This array containing color intensity added to bitmap
 * where owners should be visible.
 */
struct PALETTE_ENTRY owned_area_palette_intns[] =
 { {63,24,11,0}, {47,31,63,0}, {54,63,26,0}, {63,59,0,0}, {52,47,42,0}, {0,0,0,0}, }; /* intensified */
/**
 * Standard player colors array.
 * This array containing color intensity added to bitmap
 * where owners should be visible.
 */
struct PALETTE_ENTRY owned_area_palette_std[] =
/* { {31,12,5,0}, {20,13,27,0}, {23,27,11,0}, {31,29,0,0}, {22,19,17,0}, {0,0,0,0}, };*/ /* for sum */
 { {186,72,30,0}, {120,78,162,0}, {138,162,66,0}, {186,154,0,0}, {132,114,102,0}, {0,0,0,0}, }; /* for mul */
/**
 * Weak player colors array.
 * This array containing color intensity added to bitmap
 * where owners should be visible.
 */
struct PALETTE_ENTRY owned_area_palette_weak[] =
/* { {19,7,2,0}, {10,6,15,0}, {11,14,5,0}, {15,14,0,0}, {11,9,8,0}, {0,0,0,0}, };*/ /* for sum */
 { {62,24,10,0}, {40,26,54,0}, {46,54,22,0}, {62,55,0,0}, {44,38,34,0}, {0,0,0,0}, };/* for mul */

/**
 * Intense slab colors array.
 * This array containing color intensity added to bitmap
 * where slab colors should be intensified.
 */
struct PALETTE_ENTRY intense_slab_palette_intns[] =
/* { {31,10,17,0}, {28,28,2,0}, };*/ /* for sum */
 { {248,80,136,0}, {224,224,16,0}, }; /* for mul */
/**
 * Standard slab colors array.
 * This array containing color intensity added to bitmap
 * where slab colors should be intensified.
 */
struct PALETTE_ENTRY intense_slab_palette_std[] =
/* { {15,5,8,0}, {14,14,1,0}, };*/ /* for sum */
 { {124,40,68,0}, {112,112,8,0}, }; /* for mul */

#define THINGCIRCLE_PALETTE_SIZE 14

/**
 * Minimap player ground colors array.
 */
struct PALETTE_ENTRY minimap_owned_ground[] =
 { {208,84,16,0}, {216,180,232,0}, {96,152,28,0}, {208,208,16,0}, {208,216,160,0}, {216,216,216,0}, };

/**
 * Minimap player room colors array.
 */
struct PALETTE_ENTRY minimap_owned_room[] =
 { {244,88,16,0}, {248,216,252,0}, {100,180,36,0}, {252,252,16,0}, {252,248,196,0}, {255,255,255,0}, };

/**
 * Minimap slab colors array.
 */
struct PALETTE_ENTRY minimap_walldoor[] =
 /* wall           opened door      locked door     */
 { {112,88,60,0}, {224,236,136,0}, {252,208,152,0}, };

/**
 * Minimap slab colors array.
 */
struct PALETTE_ENTRY minimap_pathliquid[] =
 /* path          water            lava          */
 { {96,72,28,0}, {140,112,104,0}, {100,60,16,0}, };

/**
 * Minimap slab variations colors array.
 */
struct PALETTE_ENTRY minimap_gold[] =
 { {140,136,36,0}, {140,136,36,0}, {160,132,36,0}, {160,136,40,0}, {140,128,28,0}, };

/**
 * Minimap slab variations colors array.
 */
struct PALETTE_ENTRY minimap_rock[] =
 { {16,16,16,0}, {20,12,16,0}, {12,20,16,0}, {16,12,20,0}, {18,14,18,0}, };

/**
 * Minimap slab variations colors array.
 */
struct PALETTE_ENTRY minimap_earth[] =
 { {56,48,16,0}, {48,44,16,0}, {60,48,16,0}, {72,50,16,0}, {72,56,16,0}, };

struct PALETTE_ENTRY thingcircle_palette_std[] =
 { {176,176,176,0},   /* 0 none */
   {224,224,  0,0},   /* 1 item */
   {255,255,255,0},   /* 2 shot */
   {  0, 96,255,0},   /* 3 effect element */
   { 16,255, 16,0},   /* 4 dead creature */
   { 16,255, 16,0},   /* 5 creature */
   {176,176,176,0},   /* 6 unknown */
   { 48, 96,255,0},   /* 7 effect generator */
   {255, 32, 32,0},   /* 8 trap */
   {255,  0,255,0},   /* 9 door */
   {176,176,176,0},   /* 10 unknown */
   {176,176,176,0},   /* 11 unknown */
   {  0, 96,255,0},   /* 12 ambient sound */
   {  0, 96,255,0},   /* 13 cave in */
   };

struct PALETTE_ENTRY thingcircle_palette_weak[] =
 { { 88, 88, 88,0},   /* 0 none */
   { 48, 48,  8,0},   /* 1 item */
   {128,128,128,0},   /* 2 shot */
   {  0, 48,128,0},   /* 3 effect element */
   {  8,128,  8,0},   /* 4 dead creature */
   {  8,128,  8,0},   /* 5 creature */
   { 88, 88, 88,0},   /* 6 unknown */
   {  8,  8,128,0},   /* 7 effect generator */
   {128, 16, 16,0},   /* 8 trap */
   {128,  0,128,0},   /* 9 door */
   { 88, 88, 88,0},   /* 10 unknown */
   { 88, 88, 88,0},   /* 11 unknown */
   {  0, 48,128,0},   /* 12 ambient sound */
   {  0, 48,128,0},   /* 13 cave in */
   };

/**
 * Weak slab colors array.
 * This array containing color intensity added to bitmap
 * where slab colors should be intensified.
 */
struct PALETTE_ENTRY intense_slab_palette_weak[] =
/* { {8,3,4,0}, {7,7,0,0}, }; */ /* for sum */
 { {62,20,34,0}, {56,56,4,0}, }; /* for mul */

const char *palette_fname="palette.dat";
const char *cube_fname="cube.dat";
const char *tmapanim_fname="tmapanim.dat";

struct MAPDRAW_DATA *glb_draw_data;

void mdrand_setpos(struct MAPDRAW_DATA *draw_data,int sx,int sy)
{
    draw_data->rand_subtl.x=sx;
    draw_data->rand_subtl.y=sy;
    draw_data->rand_count=0;
    glb_draw_data=draw_data;
}

unsigned int mdrand_t8(struct MAPDRAW_DATA *draw_data,int tx,int ty,const unsigned int range)
{
    if ((draw_data->rand_subtl.x!=tx*MAP_SUBNUM_X) || (draw_data->rand_subtl.y!=ty*MAP_SUBNUM_Y))
    {
        draw_data->rand_subtl.x=tx*MAP_SUBNUM_X;
        draw_data->rand_subtl.y=ty*MAP_SUBNUM_Y;
        draw_data->rand_count=0;
        glb_draw_data=draw_data;
    }
    int idx=((ty*MAP_SUBNUM_Y)*draw_data->subsize.x + tx*MAP_SUBNUM_X)*sizeof(int)+draw_data->rand_count;
    draw_data->rand_count++;
    return (draw_data->rand_pool[idx%draw_data->rand_size]%range);
}

unsigned int mdrand_s8(struct MAPDRAW_DATA *draw_data,int sx,int sy,const unsigned int range)
{
    if ((draw_data->rand_subtl.x!=sx) || (draw_data->rand_subtl.y!=sy))
    {
        draw_data->rand_subtl.x=sx;
        draw_data->rand_subtl.y=sy;
        draw_data->rand_count=0;
        glb_draw_data=draw_data;
    }
    int idx=((sy)*draw_data->subsize.x + sx)*sizeof(int)+draw_data->rand_count;
    draw_data->rand_count++;
    return (draw_data->rand_pool[idx%draw_data->rand_size]%range);
}

unsigned int mdrand_nx8(struct MAPDRAW_DATA *draw_data,const unsigned int range)
{
    int idx=((draw_data->rand_subtl.y)*draw_data->subsize.x + draw_data->rand_subtl.x)*sizeof(int)+draw_data->rand_count;
    draw_data->rand_count++;
    return (draw_data->rand_pool[idx%draw_data->rand_size]%range);
}

unsigned int mdrand_g8(const unsigned int range)
{
    int idx=((glb_draw_data->rand_subtl.y)*glb_draw_data->subsize.x + glb_draw_data->rand_subtl.x)*sizeof(int)+glb_draw_data->rand_count;
    glb_draw_data->rand_count++;
    return (glb_draw_data->rand_pool[idx%glb_draw_data->rand_size]%range);
}

void mdrand_g8_waste(const unsigned int num)
{
    glb_draw_data->rand_count+=num;
}


/**
 * Reads RGB palette file into PALETTE_ENTRY array.
 * Uses MEMORY_FILE for reading - accepts compressed input files.
 * @param pal The destination array of palette entries.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_palette(struct PALETTE_ENTRY *pal,char *fname)
{
    message_log(" load_palette: Starting");
    /* Reading file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /* Checking file size */
    if ((mem->len!=256*3))
    {
      memfile_free(&mem);
      return ERR_FILE_BADDATA;
    }
    /* Loading the entries */
    int i;
    for (i=0; i<256; i++)
    {
        unsigned int item_pos=i*3;
        pal[i].b=mem->content[item_pos+2];
        pal[i].g=mem->content[item_pos+1];
        pal[i].r=mem->content[item_pos+0];
        pal[i].o=0;
    }
    memfile_free(&mem);
    message_log(" load_palette: Finished");
    return ERR_NONE;
}

/**
 * Draws one pixel at given offset of destination buffer.
 * Inserts exact PALETTE_ENTRY color values into destination buffer.
 * @param dest The destination buffer.
 * @param offset Position in destination buffer of the pixel.
 * @param color Pixel color.
 */
void draw_pixel_exact_offs(unsigned char *dest,
    const unsigned long offset,const struct PALETTE_ENTRY *color)
{
    dest[offset+0]=color->b;
    dest[offset+1]=color->g;
    dest[offset+2]=color->r;
}

/**
 * Multiplies color of one pixel at given offset of destination buffer.
 * Multiplies PALETTE_ENTRY color values with previous pixel values.
 * @param dest The destination buffer.
 * @param offset Position in destination buffer of the pixel.
 * @param color Pixel color.
 */
void draw_pixel_mul_offs(unsigned char *dest,
    const unsigned long offset,const struct PALETTE_ENTRY *color)
{
  unsigned short nvalr,nvalg,nvalb;
  nvalb=(dest[offset+0]);
  nvalg=(dest[offset+1]);
  nvalr=(dest[offset+2]);
  unsigned short sum=nvalr+nvalg+nvalb;
  dest[offset+0]=min(nvalb+((sum*color->b)>>10),255);
  dest[offset+1]=min(nvalg+((sum*color->g)>>10),255);
  dest[offset+2]=min(nvalr+((sum*color->r)>>10),255);
}

/**
 * Puts color of one pixel at given offset of destination buffer, uses alpha channel.
 * Adds PALETTE_ENTRY color values with previous pixel values, using alpha as factor.
 * @param dest The destination buffer.
 * @param offset Position in destination buffer of the pixel.
 * @param alpha Pixel alpha.
 * @param color Pixel color.
 */
void draw_pixel_x4walpha_offs(unsigned char *dest,
    const unsigned long offset,const unsigned short alpha,
    const struct PALETTE_ENTRY *color)
{
  unsigned short nvalr,nvalg,nvalb;
  nvalb=(dest[offset+0]);
  nvalg=(dest[offset+1]);
  nvalr=(dest[offset+2]);
  dest[offset+0]=(alpha*nvalb+(255-alpha)*(color->b<<2))>>8;
  dest[offset+1]=(alpha*nvalg+(255-alpha)*(color->g<<2))>>8;
  dest[offset+2]=(alpha*nvalr+(255-alpha)*(color->r<<2))>>8;
}

/**
 * Draws filled circle on given buffer.
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the circle center.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param bcolor Border color.
 * @param fcolor Fill color.
 * @param radius Circle radius.
 */
void draw_circle_fill(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size,const unsigned int dest_scanln,
    const struct PALETTE_ENTRY *bcolor,const struct PALETTE_ENTRY *fcolor,
    int radius)
{
  unsigned long n=0;
  long invradius=(1/(float)radius)*0x10000L;
  int dx=0,dy=radius;
  if ((dy>15)||(dy==1)) dy--;
  int i;
  int dxoffset,dyoffset;
  unsigned int offset = dest_pos.y*dest_scanln;

  while (dx<=dy)
  {
      dxoffset = dest_scanln*dx;
      dyoffset = dest_scanln*dy;
      /* draw border */
      i=dy;
      {
        int pos;
        if ((dest_pos.y-dx>=0)&&(dest_pos.y-dx<dest_size.y))
        {
          pos=dest_pos.x+i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos-dxoffset,bcolor);  /* octant 0 */
          pos=dest_pos.x-i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos-dxoffset,bcolor);  /* octant 3 */
        }
        if ((dest_pos.y-i>=0)&&(dest_pos.y-i<dest_size.y))
        {
          pos=dest_pos.x+dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos-dyoffset,bcolor);  /* octant 1 */
          pos=dest_pos.x-dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos-dyoffset,bcolor);  /* octant 2 */
        }
        if ((dest_pos.y+dx>=0)&&(dest_pos.y+dx<dest_size.y))
        {
          pos=dest_pos.x-i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos+dxoffset,bcolor);  /* octant 4 */
          pos=dest_pos.x+i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos+dxoffset,bcolor);  /* octant 7 */
        }
        if ((dest_pos.y+i>=0)&&(dest_pos.y+i<dest_size.y))
        {
          pos=dest_pos.x-dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos+dyoffset,bcolor);  /* octant 5 */
          pos=dest_pos.x+dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos+dyoffset,bcolor);  /* octant 6 */
        }
      }
      i--;dyoffset-=dest_scanln;
      /* draw inside */
      for(;i>=dx;i--,dyoffset-=dest_scanln)
      {
        int pos;
        if ((dest_pos.y-dx>=0)&&(dest_pos.y-dx<dest_size.y))
        {
          pos=dest_pos.x+i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos-dxoffset,fcolor);  /* octant 0 */
          pos=dest_pos.x-i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos-dxoffset,fcolor);  /* octant 3 */
        }
        if ((dest_pos.y-i>=0)&&(dest_pos.y-i<dest_size.y))
        {
          pos=dest_pos.x+dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos-dyoffset,fcolor);  /* octant 1 */
          pos=dest_pos.x-dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos-dyoffset,fcolor);  /* octant 2 */
        }
        if ((dest_pos.y+dx>=0)&&(dest_pos.y+dx<dest_size.y))
        {
          pos=dest_pos.x-i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos+dxoffset,fcolor);  /* octant 4 */
          pos=dest_pos.x+i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos+dxoffset,fcolor);  /* octant 7 */
        }
        if ((dest_pos.y+i>=0)&&(dest_pos.y+i<dest_size.y))
        {
          pos=dest_pos.x-dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos+dyoffset,fcolor);  /* octant 5 */
          pos=dest_pos.x+dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_exact_offs(dest,offset+3*pos+dyoffset,fcolor);  /* octant 6 */
        }
      }
      {
        dx++;
        n+=invradius;
        if ((n>>6)>=SIN_ACOS_SIZE) break;
        dy = (int)((radius * (glb_draw_data->sin_acos[(int)(n>>6)])) >> 16);
      }
  }
} 

/**
 * Draws filled circle on given buffer by multiplying current pixel values.
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the circle center.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param bcolor Border color factors.
 * @param fcolor Fill color factors.
 * @param radius Circle radius.
 */
void draw_circle_mul(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size,const unsigned int dest_scanln,
    const struct PALETTE_ENTRY *bcolor,const struct PALETTE_ENTRY *fcolor,
    int radius)
{
  unsigned long n=0;
  long invradius=(1/(float)radius)*0x10000L;
  int dx=0,dy=radius;
  if ((dy>15)||(dy==1)) dy--;
  int i;
  int dxoffset,dyoffset;
  unsigned int offset = dest_pos.y*dest_scanln;

  while (dx<=dy)
  {
      dxoffset = dest_scanln*dx;
      dyoffset = dest_scanln*dy;
      /* draw border */
      i=dy;
      {
        int pos;
        if ((dest_pos.y-dx>=0)&&(dest_pos.y-dx<dest_size.y))
        {
          pos=dest_pos.x+i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos-dxoffset,bcolor);  /* octant 0 */
          pos=dest_pos.x-i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos-dxoffset,bcolor);  /* octant 3 */
        }
        if ((dest_pos.y-i>=0)&&(dest_pos.y-i<dest_size.y))
        {
          pos=dest_pos.x+dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos-dyoffset,bcolor);  /* octant 1 */
          pos=dest_pos.x-dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos-dyoffset,bcolor);  /* octant 2 */
        }
        if ((dest_pos.y+dx>=0)&&(dest_pos.y+dx<dest_size.y))
        {
          pos=dest_pos.x-i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos+dxoffset,bcolor);  /* octant 4 */
          pos=dest_pos.x+i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos+dxoffset,bcolor);  /* octant 7 */
        }
        if ((dest_pos.y+i>=0)&&(dest_pos.y+i<dest_size.y))
        {
          pos=dest_pos.x-dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos+dyoffset,bcolor);  /* octant 5 */
          pos=dest_pos.x+dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos+dyoffset,bcolor);  /* octant 6 */
        }
      }
      i--;dyoffset-=dest_scanln;
      /* draw inside */
      for(;i>=dx;i--,dyoffset-=dest_scanln)
      {
        int pos;
        if ((dest_pos.y-dx>=0)&&(dest_pos.y-dx<dest_size.y))
        {
          pos=dest_pos.x+i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos-dxoffset,fcolor);  /* octant 0 */
          pos=dest_pos.x-i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos-dxoffset,fcolor);  /* octant 3 */
        }
        if ((dest_pos.y-i>=0)&&(dest_pos.y-i<dest_size.y))
        {
          pos=dest_pos.x+dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos-dyoffset,fcolor);  /* octant 1 */
          pos=dest_pos.x-dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos-dyoffset,fcolor);  /* octant 2 */
        }
        if ((dest_pos.y+dx>=0)&&(dest_pos.y+dx<dest_size.y))
        {
          pos=dest_pos.x-i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos+dxoffset,fcolor);  /* octant 4 */
          pos=dest_pos.x+i ;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos+dxoffset,fcolor);  /* octant 7 */
        }
        if ((dest_pos.y+i>=0)&&(dest_pos.y+i<dest_size.y))
        {
          pos=dest_pos.x-dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos+dyoffset,fcolor);  /* octant 5 */
          pos=dest_pos.x+dx;
          if ((pos>=0)&&(pos<dest_size.x))
            draw_pixel_mul_offs(dest,offset+3*pos+dyoffset,fcolor);  /* octant 6 */
        }
      }
      {
        dx++;
        n+=invradius;
        if ((n>>6)>=SIN_ACOS_SIZE) break;
        dy = (int)((radius * (glb_draw_data->sin_acos[(int)(n>>6)])) >> 16);
      }
  }
} 

/**
 * Places given sprite on RGB buffer, centered on given position.
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the sprite center.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param pal Sprite palette.
 * @param spr The sprite to place on buffer.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short place_sprite_cntr_on_buf_rgb(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size,const unsigned int dest_scanln,
    const struct PALETTE_ENTRY *pal,const struct IMAGEITEM *spr)
{
    long dest_idx=((dest_pos.y-(spr->height>>1))*dest_scanln);
    unsigned long src_idx=0;
    unsigned long dest_fullsize=dest_scanln*dest_size.y;
    unsigned long dest_startx=3*(dest_pos.x-(spr->width>>1));
    unsigned long dest_maxidx=(dest_size.x-1)*3;
    if (dest_maxidx>=dest_scanln) dest_maxidx=dest_scanln-1;
    int w,h;
    for (h=0;h<spr->height;h++)
    {
      if (dest_idx>=0) break;
      dest_idx+=dest_scanln;
      src_idx+=spr->width;
    }
    for (;h<spr->height;h++)
    {
      if (dest_idx>=dest_fullsize) break;
      long dest_sidx=dest_startx;
      for (w=0;w<spr->width;w++)
      {
          if (dest_sidx>=0) break;
          dest_sidx+=3;
      }
      for (;w<spr->width;w++)
      {
          if (dest_sidx>dest_maxidx) continue;
          /* Using multiplication to place colour on the pixel */
          draw_pixel_x4walpha_offs(dest,dest_idx+dest_sidx,
              spr->alpha[src_idx+w],&pal[spr->data[src_idx+w]]);
          dest_sidx+=3;
      }
      dest_idx+=dest_scanln;
      src_idx+=spr->width;
    }
    return ERR_NONE;
}

/**
 * Multiplies values in buffer with given factors.
 * Note that this is not clear multiplication, but multiplication+sum.
 * @param dest The buffer.
 * @param dest_pos Position in buffer of the affected rectangle top left.
 * @param dest_size Dimensions of buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param rect_size Dimensions of affected rectangle.
 * @param pxdata Multiply factors.
 * @param scale Buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_rect_mul_on_buffer(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size,const unsigned int dest_scanln,
    const struct IPOINT_2D rect_size,const struct PALETTE_ENTRY *pxdata,
    const struct IPOINT_2D scale)
{
    unsigned long dest_idx=(dest_pos.y*dest_scanln);
    unsigned long dest_fullsize=dest_scanln*dest_size.y;
    unsigned long dest_startx=3*dest_pos.x;
    unsigned long dest_maxidx=(dest_size.x-1)*3;
    if (dest_maxidx>=dest_scanln) dest_maxidx=dest_scanln-1;
    struct IPOINT_2D dest_rect_size={rect_size.x/scale.x,rect_size.y/scale.y};
/*    struct IPOINT_2D dest_rect_size=
        {rect_size.x/scale.x+((rect_size.x%scale.x)>0),
         rect_size.y/scale.y+((rect_size.y%scale.y)>0)};*/
    int i,j;
    for (j=0;j<dest_rect_size.y;j++)
    {
      if (dest_idx>=dest_fullsize) break;
      unsigned long dest_sidx=dest_startx;
      for (i=0;i<dest_rect_size.x;i++)
      {
          if (dest_sidx>dest_maxidx) continue;
          /* Using multiplication to place colour on the pixel */
          draw_pixel_mul_offs(dest,dest_idx+dest_sidx,pxdata);
          dest_sidx+=3;
      }
      dest_idx+=dest_scanln;
    }
    return ERR_NONE;
}

/**
 * Adds given factors to values in buffer.
 * @param dest The buffer.
 * @param dest_pos Position in buffer of the affected rectangle top left.
 * @param dest_size Dimensions of buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param rect_size Dimensions of affected rectangle.
 * @param pxdata Values to sum with.
 * @param scale Buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_rect_sum_on_buffer(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size,const unsigned int dest_scanln,
    const struct IPOINT_2D rect_size,const struct PALETTE_ENTRY *pxdata,
    const struct IPOINT_2D scale)
{
    unsigned long dest_idx=(dest_pos.y*dest_scanln);
    unsigned long dest_fullsize=dest_scanln*dest_size.y;
    int i,j;
    for (j=0;j<rect_size.y;j++)
    {
      if (dest_idx>=dest_fullsize) break;
      if ((j%scale.y)>0)
          continue;
      for (i=0;i<rect_size.x;i++)
      {
          if ((i%scale.x)!=0) continue;
          unsigned long dest_sidx=dest_pos.x+(i/scale.x);
          if (dest_sidx>=dest_size.x) continue;
          /* Using sum place colour on the pixel */
          unsigned char nval;
          dest_sidx*=3;
          nval=(dest[dest_idx+dest_sidx+0]);
          dest[dest_idx+dest_sidx+0]=min(nval+pxdata->b,255);
          nval=(dest[dest_idx+dest_sidx+1]);
          dest[dest_idx+dest_sidx+1]=min(nval+pxdata->g,255);
          nval=(dest[dest_idx+dest_sidx+2]);
          dest[dest_idx+dest_sidx+2]=min(nval+pxdata->r,255);
      }
      dest_idx+=dest_scanln;
    }
    return ERR_NONE;
}

/**
 * Draws given texture on RGB buffer. Optimized for highly rescaled textures.
 * Requires the scale factor to be at least 8 (otherwise the picture may be blurred).
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer_avg4(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    long dest_idx=(dest_pos.y*dest_scanln);
    long src_idx=(src_pos.y*src_size.x);
    unsigned long src_fullsize=src_size.x*src_size.y;
    unsigned long dest_fullsize=dest_scanln*dest_size.y;
    unsigned long dest_startx=3*dest_pos.x;
    unsigned long dest_maxidx=(dest_size.x-1)*3;
    if (dest_maxidx>=dest_scanln) dest_maxidx=dest_scanln-1;
    int i,j;
    for (j=0;j<rect_size.y;j++)
    {
        if ((src_idx>=0)&&(dest_idx>=0))
            break;
        if ((j%scale.y)>0)
        {
            src_idx+=src_size.x;
            continue;
        }
        unsigned short ridy=mdrand_g8(scale.x);
        for (i=0;i<rect_size.x;i++)
          if ((i%scale.x)==(ridy))
          {
              i+=scale.x-ridy-1;
              ridy=mdrand_g8(scale.x);
              mdrand_g8_waste(2);
          }
        dest_idx+=dest_scanln;
        src_idx+=src_size.x;
    }
    for (;j<rect_size.y;j++)
    {
      if ((j%scale.y)>0)
      {
          src_idx+=src_size.x;
          continue;
      }
      if (dest_idx>=dest_fullsize) break;
      if (src_idx>=src_fullsize) break;
      unsigned short ridy=mdrand_g8(scale.x);
      /* Determine if we won't be out of source bounds for max value of src_add */
      if (src_idx+(scale.y-1)*src_size.x>=src_fullsize) break;
      unsigned long dest_sidx=dest_startx;
      for (i=0;i<rect_size.x;i++)
      {
          if ((i%scale.x)!=(ridy)) continue;
          int src_xfinal=src_pos.x+i;
          i+=scale.x-ridy-1;
          ridy=mdrand_g8(scale.x);
          /* Select two source lines */
          int scaley_half=scale.y>>1;
          int src_add1=(mdrand_g8(scaley_half))*src_size.x;
          int src_add2=(mdrand_g8(scaley_half)+scaley_half)*src_size.x;
          if (dest_sidx>dest_maxidx) continue;
          if (src_xfinal>=src_size.x) continue;
          /* Getting pixels for the average */
          struct PALETTE_ENTRY *pxdata1;
          struct PALETTE_ENTRY *pxdata2;
          struct PALETTE_ENTRY *pxdata3;
          struct PALETTE_ENTRY *pxdata4;
          if ((src_xfinal+6)>=src_size.x)
          {
            pxdata1=&pal[src[src_idx+src_add1+src_xfinal]];
            pxdata2=pxdata1;
            pxdata3=pxdata1;
            pxdata4=pxdata1;
          } else
          {
            pxdata1=&pal[src[src_idx+src_add1+src_xfinal]];
            pxdata2=&pal[src[src_idx+src_add1+src_xfinal+2]];
            pxdata3=&pal[src[src_idx+src_add2+src_xfinal+4]];
            pxdata4=&pal[src[src_idx+src_add2+src_xfinal+6]];
          }
          dest[dest_idx+dest_sidx+0]=(pxdata1->b)+(pxdata2->b)+(pxdata3->b)+(pxdata4->b);
          dest[dest_idx+dest_sidx+1]=(pxdata1->g)+(pxdata2->g)+(pxdata3->g)+(pxdata4->g);
          dest[dest_idx+dest_sidx+2]=(pxdata1->r)+(pxdata2->r)+(pxdata3->r)+(pxdata4->r);
          dest_sidx+=3;
      }
      dest_idx+=dest_scanln;
      src_idx+=src_size.x;
    }
    return ERR_NONE;
}

/**
 * Draws given texture on RGB buffer. Optimized for highly rescaled textures.
 * Requires the scale factor to be at least 8 (otherwise the picture may be blurred).
 * Unsafe version - won't check if dest buffer is our of bounds.
 * Do not use it for partially displayed slabs, or it'll crash!
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer_avg4_unsafe(unsigned char *dest,const struct IPOINT_2D dest_pos,
    __attribute__((unused)) const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    long dest_idx=(dest_pos.y*dest_scanln);
    long src_idx=(src_pos.y*src_size.x);
    int i,j;
    for (j=0;j<rect_size.y;j++)
    {
      if ((j%scale.y)>0)
      {
          src_idx+=src_size.x;
          continue;
      }
      unsigned short ridy=mdrand_g8(scale.x);
      /* Determine if we won't be out of source bounds for max value of src_add */
      for (i=0;i<rect_size.x;i++)
      {
          if ((i%scale.x)!=(ridy)) continue;
          int src_xfinal=src_pos.x+i;
          i+=scale.x-ridy-1;
          unsigned long dest_sidx=dest_pos.x+(i/scale.x);
          ridy=mdrand_g8(scale.x);
          /* Select two source lines */
          int scaley_half=scale.y>>1;
          int src_add1=(mdrand_g8(scaley_half))*src_size.x;
          int src_add2=(mdrand_g8(scaley_half)+scaley_half)*src_size.x;
          src_add1+=src_idx+src_xfinal;
          src_add2+=src_idx+src_xfinal;
          /* Getting pixels for the average */
          struct PALETTE_ENTRY *pxdata1;
          struct PALETTE_ENTRY *pxdata2;
          struct PALETTE_ENTRY *pxdata3;
          struct PALETTE_ENTRY *pxdata4;
          pxdata1=&pal[src[src_add1]];
          pxdata2=&pal[src[src_add1+2]];
          pxdata3=&pal[src[src_add2+4]];
          pxdata4=&pal[src[src_add2+6]];
          dest_sidx*=3;
          dest_sidx+=dest_idx;
          dest[dest_sidx+0]=(pxdata1->b)+(pxdata2->b)+(pxdata3->b)+(pxdata4->b);
          dest[dest_sidx+1]=(pxdata1->g)+(pxdata2->g)+(pxdata3->g)+(pxdata4->g);
          dest[dest_sidx+2]=(pxdata1->r)+(pxdata2->r)+(pxdata3->r)+(pxdata4->r);
      }
      dest_idx+=dest_scanln;
      src_idx+=src_size.x;
    }
    return ERR_NONE;
}

/**
 * Draws given texture on RGB buffer. Optimized for medium rescaled textures.
 * Requires the scale factor to be at least 4 (otherwise the picture may be blurred).
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer_avg2(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    long dest_idx=(dest_pos.y*dest_scanln);
    long src_idx=(src_pos.y*src_size.x);
    unsigned long src_fullsize=src_size.x*src_size.y;
    unsigned long dest_fullsize=dest_scanln*dest_size.y;
    int i,j;
    for (j=0;j<rect_size.y;j++)
    {
        if ((src_idx>=0)&&(dest_idx>=0))
            break;
        if ((j%scale.y)>0)
        {
            src_idx+=src_size.x;
            continue;
        }
        unsigned short ridy=mdrand_g8(scale.x);
        for (i=0;i<rect_size.x;i++)
          if ((i%scale.x)==(ridy))
          {
              i+=scale.x-ridy-1;
              ridy=mdrand_g8(scale.x);
              mdrand_g8_waste(1);
          }
        dest_idx+=dest_scanln;
        src_idx+=src_size.x;
    }
    for (;j<rect_size.y;j++)
    {
      if ((j%scale.y)>0)
      {
          src_idx+=src_size.x;
          continue;
      }
      if (dest_idx>=dest_fullsize) break;
      if (src_idx>=src_fullsize) break;
      unsigned short ridy=mdrand_g8(scale.x);
      /* Determine if we won't be out of source bounds for max value of src_add */
      if (src_idx+(scale.y-1)*src_size.x>=src_fullsize) break;
      for (i=0;i<rect_size.x;i++)
      {
          if ((i%scale.x)!=(ridy)) continue;
          int src_xfinal=src_pos.x+i;
          i+=scale.x-ridy-1;
          unsigned long dest_sidx=dest_pos.x+(i/scale.x);
          ridy=mdrand_g8(scale.x);
          int src_add=mdrand_g8(scale.y)*src_size.x;
          if (dest_sidx>=dest_size.x) continue;
          if ((src_pos.x+i)>=src_size.x) continue;
          struct PALETTE_ENTRY *pxdata1;
          struct PALETTE_ENTRY *pxdata2;
          if ((src_xfinal+2)>=src_size.x)
          {
            pxdata1=&pal[src[src_idx+src_add+src_xfinal]];
            pxdata2=pxdata1;
          } else
          {
            pxdata1=&pal[src[src_idx+src_add+src_xfinal]];
            pxdata2=&pal[src[src_idx+src_add+src_xfinal+2]];
          }
          dest_sidx*=3;
          dest_sidx+=dest_idx;
          dest[dest_sidx+0]=(pxdata1->b+pxdata2->b)<<1;
          dest[dest_sidx+1]=(pxdata1->g+pxdata2->g)<<1;
          dest[dest_sidx+2]=(pxdata1->r+pxdata2->r)<<1;
      }
      dest_idx+=dest_scanln;
      src_idx+=src_size.x;
    }
    return ERR_NONE;
}

/**
 * Draws given texture on RGB buffer. Fast version, optimized for medium rescaled textures.
 * Requires the scale factor to be at least 4 (otherwise the picture may be blurred).
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
FASTCALL short draw_texture_on_buffer_avg2_fast(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    long dest_idx=(dest_pos.y*dest_scanln);
    long src_idx=(src_pos.y*src_size.x);
    unsigned long dest_fullsize=dest_scanln*dest_size.y;
    int i,j;
    for (j=0;j<rect_size.y;j++)
    {
        if ((src_idx>=0)&&(dest_idx>=0))
            break;
        if ((j%scale.y)>0)
        {
            src_idx+=src_size.x;
            continue;
        }
        unsigned short ridy=mdrand_g8(scale.x);
        for (i=0;i<rect_size.x;i++)
          if ((i%scale.x)==(ridy)) mdrand_g8_waste(1);
        dest_idx+=dest_scanln;
        src_idx+=src_size.x;
    }
    for (;j<rect_size.y;j++)
    {
      if ((j%scale.y)>0)
      {
          src_idx+=src_size.x;
          continue;
      }
      if (dest_idx>=dest_fullsize) break;
      unsigned short ridy=mdrand_g8(scale.x);
      long dest_sidx=3*dest_pos.x;
      for (i=0;i<rect_size.x;i++)
      {
          if (dest_sidx>=0) break;
          if ((i%scale.x)!=(ridy)) continue;
          mdrand_g8_waste(1);
          dest_sidx+=3;
      }
      for (;i<rect_size.x;i++)
      {
          if ((i%scale.x)!=(ridy)) continue;
          int src_add=mdrand_g8(scale.y)*src_size.x;
          if (dest_sidx+2>=dest_scanln) continue;
          src_add+=src_idx+src_pos.x+i;
          struct PALETTE_ENTRY *pxdata1;
          struct PALETTE_ENTRY *pxdata2;
          pxdata1=&pal[src[src_add]];
          pxdata2=&pal[src[src_add+2]];
          dest[dest_idx+dest_sidx+0]=(pxdata1->b+pxdata2->b)<<1;
          dest[dest_idx+dest_sidx+1]=(pxdata1->g+pxdata2->g)<<1;
          dest[dest_idx+dest_sidx+2]=(pxdata1->r+pxdata2->r)<<1;
          dest_sidx+=3;
      }
      dest_idx+=dest_scanln;
      src_idx+=src_size.x;
    }
    return ERR_NONE;
}

/**
 * Draws given texture on RGB buffer. Fast version, optimized for medium rescaled textures.
 * Requires the scale factor to be at least 4 (otherwise the picture may be blurred).
 * Unsafe version - won't check if dest buffer is our of bounds.
 * Do not use it for partially displayed slabs, or it'll crash!
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
FASTCALL short draw_texture_on_buffer_avg2_fast_unsafe(unsigned char *dest,const struct IPOINT_2D dest_pos,
    __attribute__((unused)) const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    long dest_idx=(dest_pos.y*dest_scanln);
    long src_idx=(src_pos.y*src_size.x);
    int i,j;
    for (j=0;j<rect_size.y;j++)
    {
      if ((j%scale.y)>0)
      {
          src_idx+=src_size.x;
          continue;
      }
      unsigned short ridy=mdrand_g8(scale.x);
      long dest_sidx=3*dest_pos.x;
      for (i=0;i<rect_size.x;i++)
      {
          if ((i%scale.x)!=(ridy)) continue;
          int src_add=mdrand_g8(scale.y)*src_size.x;
          src_add+=src_idx+src_pos.x+i;
          struct PALETTE_ENTRY *pxdata1;
          struct PALETTE_ENTRY *pxdata2;
          pxdata1=&pal[src[src_add]];
          pxdata2=&pal[src[src_add+2]];
          dest[dest_idx+dest_sidx+0]=(pxdata1->b+pxdata2->b)<<1;
          dest[dest_idx+dest_sidx+1]=(pxdata1->g+pxdata2->g)<<1;
          dest[dest_idx+dest_sidx+2]=(pxdata1->r+pxdata2->r)<<1;
          dest_sidx+=3;
      }
      dest_idx+=dest_scanln;
      src_idx+=src_size.x;
    }
    return ERR_NONE;
}

/**
 * Draws given texture on RGB buffer. Optimized for low rescaled textures.
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer_noavg(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    long dest_idx=(dest_pos.y*dest_scanln);
    long src_idx=(src_pos.y*src_size.x);
    unsigned long src_fullsize=src_size.x*src_size.y;
    unsigned long dest_fullsize=dest_scanln*dest_size.y;
    unsigned long dest_maxidx=(dest_size.x-1)*3;
    if (dest_maxidx>=dest_scanln) dest_maxidx=dest_scanln-1;
    int i,j;
    for (j=0;j<rect_size.y;j++)
    {
        if ((src_idx>=0)&&(dest_idx>=0))
            break;
        if ((j%scale.y)>0)
        {
            src_idx+=src_size.x;
            continue;
        }
        unsigned short ridy=mdrand_g8(scale.x);
        for (i=0;i<rect_size.x;i++)
        {
            if ((i%scale.x)==(ridy))
            {
                i+=scale.x-ridy-1;
                ridy=mdrand_g8(scale.x);
                mdrand_g8_waste(1);
            }
        }        
        dest_idx+=dest_scanln;
        src_idx+=src_size.x;
    }
    for (;j<rect_size.y;j++)
    {
        if ((j%scale.y)>0)
        {
            src_idx+=src_size.x;
            continue;
        }
        unsigned short ridy=mdrand_g8(scale.x);
        /* The bound conditions are rarely true - this is why they are here, not before */
        if (dest_idx >= dest_fullsize) break;
        if (src_idx+(scale.y-1)*src_size.x >= src_fullsize) break;
        long dest_sidx=3*dest_pos.x;
        for (i=0;i<rect_size.x;i++)
        {
            if (dest_sidx>=0) break;
            if ((i%scale.x)==(ridy))
            {
              i+=scale.x-ridy-1;
              ridy=mdrand_g8(scale.x);
              mdrand_g8_waste(1);
              dest_sidx+=3;
            }
        }
        for (;i<rect_size.x;i++)
        {
            if ((i%scale.x)!=(ridy)) continue;
            int src_xfinal=src_pos.x+i;
            i+=scale.x-ridy-1;
            ridy=mdrand_g8(scale.x);
            int src_add=src_idx+mdrand_g8(scale.y)*src_size.x;
            if (dest_sidx>dest_maxidx) continue;
            if (src_xfinal>=src_size.x) continue;
            struct PALETTE_ENTRY *pxdata=&pal[src[src_add+src_xfinal]];
            dest[dest_idx+dest_sidx+0]=(pxdata->b<<2);
            dest[dest_idx+dest_sidx+1]=(pxdata->g<<2);
            dest[dest_idx+dest_sidx+2]=(pxdata->r<<2);
            dest_sidx+=3;
        }
        dest_idx+=dest_scanln;
        src_idx+=src_size.x;
    }
    return ERR_NONE;
}

/**
 * Draws given texture on RGB buffer. Optimized for low rescaled textures.
 * Unsafe version - won't check if dest buffer is our of bounds.
 * Do not use it for partially displayed slabs, or it'll crash!
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer_noavg_unsafe(unsigned char *dest,const struct IPOINT_2D dest_pos,
    __attribute__((unused)) const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    long dest_idx=(dest_pos.y*dest_scanln);
    long src_idx=(src_pos.y*src_size.x);
    int i,j;
    /* Danger - We asume that both src_idx and dest_idx are not less than zero here! */
    for (j=0;j<rect_size.y;j++)
    {
        if ((j%scale.y)>0)
        {
            src_idx+=src_size.x;
            continue;
        }
        unsigned short ridy=mdrand_g8(scale.x);
        long dest_sidx=3*dest_pos.x;
        /* Danger - We asume that dest_sidx is not less than zero here! */
        for (i=0;i<rect_size.x;i++)
        {
            if ((i%scale.x)!=(ridy)) continue;
            int src_add=src_pos.x+i;
            i+=scale.x-ridy-1;
            ridy=mdrand_g8(scale.x);
            src_add+=src_idx+mdrand_g8(scale.y)*src_size.x;
            /* Danger - we assume that dest_sidx is less than scanline length,
               and src_pos.x+i is less or equal src_size.x here! */
            struct PALETTE_ENTRY *pxdata=&pal[src[src_add]];
            dest[dest_idx+dest_sidx+0]=(pxdata->b<<2);
            dest[dest_idx+dest_sidx+1]=(pxdata->g<<2);
            dest[dest_idx+dest_sidx+2]=(pxdata->r<<2);
            dest_sidx+=3;
        }
        dest_idx+=dest_scanln;
        src_idx+=src_size.x;
    }
    return ERR_NONE;
}

/**
 * Draws given texture on RGB buffer. Optimized for drawing without scaling.
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer_noscale(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal)
{
    long dest_idx=(dest_pos.y*dest_scanln);
    long src_idx=(src_pos.y*src_size.x);
    unsigned long src_fullsize=src_size.x*src_size.y;
    unsigned long dest_fullsize=dest_scanln*dest_size.y;
    int i,j;
    for (j=0;j<rect_size.y;j++)
    {
        if ((src_idx>=0)&&(dest_idx>=0))
            break;
        dest_idx+=dest_scanln;
        src_idx+=src_size.x;
    }
    for (;j<rect_size.y;j++)
    {
      if (dest_idx>=dest_fullsize) break;
      if (src_idx>=src_fullsize) break;
      for (i=0;i<rect_size.x;i++)
      {
          unsigned long dest_sidx=dest_pos.x+i;
          if (dest_sidx>=dest_size.x) continue;
          if ((src_pos.x+i)>=src_size.x) continue;
          struct PALETTE_ENTRY *pxdata=&pal[src[src_idx+src_pos.x+i]];
          dest_sidx*=3;
          dest_sidx+=dest_idx;
          dest[dest_sidx+0]=(pxdata->b<<2);
          dest[dest_sidx+1]=(pxdata->g<<2);
          dest[dest_sidx+2]=(pxdata->r<<2);
      }
      dest_idx+=dest_scanln;
      src_idx+=src_size.x;
    }
    return ERR_NONE;
}

/**
 * Draws given texture on RGB buffer.
 * Select best quality drawing method based on scale parameter.
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    if (scale.x>7)
        return draw_texture_on_buffer_avg4(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
    else
    if (scale.x>3)
        return draw_texture_on_buffer_avg2(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
    else
    if ((scale.x==1)&&(scale.y==1))
        return draw_texture_on_buffer_noscale(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal);
    else
        return draw_texture_on_buffer_noavg(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
}

/**
 * Draws given texture on RGB buffer.
 * Select best quality drawing method based on scale parameter.
 * Unsafe version - won't check if dest buffer is our of bounds.
 * Do not use it for partially displayed slabs, or it'll crash!
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer_unsafe(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    if (scale.x>7)
        return draw_texture_on_buffer_avg4_unsafe(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
    else
    if (scale.x>3)
        return draw_texture_on_buffer_avg2(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
    else
    if ((scale.x==1)&&(scale.y==1))
        return draw_texture_on_buffer_noscale(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal);
    else
        return draw_texture_on_buffer_noavg_unsafe(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
}

/**
 * Draws given texture on RGB buffer.
 * Selects best speed drawing method based on scale parameter.
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer_fast(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    if (scale.x>7)
        return draw_texture_on_buffer_avg2_fast(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
    else
    if ((scale.x==1)&&(scale.y==1))
        return draw_texture_on_buffer_noscale(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal);
    else
        return draw_texture_on_buffer_noavg(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
}

/**
 * Draws given texture on RGB buffer.
 * Selects best speed drawing method based on scale parameter.
 * Unsafe version - won't check if dest buffer is our of bounds.
 * Do not use it for partially displayed slabs, or it'll crash!
 * @param dest The destination buffer.
 * @param dest_pos Position in destination buffer of the texture top left.
 * @param dest_size Dimensions of destination buffer.
 * @param dest_scanln Destination buffer scanline lenght.
 * @param src Source texture buffer.
 * @param src_pos Source texture position.
 * @param src_size Source textures size.
 * @param rect_size Texture rectangle size to draw.
 * @param pal Texture palette.
 * @param scale Destination buffer scale.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_texture_on_buffer_fast_unsafe(unsigned char *dest,const struct IPOINT_2D dest_pos,
    const struct IPOINT_2D dest_size, const unsigned int dest_scanln,
    const unsigned char *src,const struct IPOINT_2D src_pos,const struct IPOINT_2D src_size,
    const struct IPOINT_2D rect_size,struct PALETTE_ENTRY *pal,const struct IPOINT_2D scale)
{
    if (scale.x>7)
        return draw_texture_on_buffer_avg2_fast_unsafe(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
    else
    if ((scale.x==1)&&(scale.y==1))
        return draw_texture_on_buffer_noscale(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal);
    else
        return draw_texture_on_buffer_noavg_unsafe(dest,dest_pos,dest_size,
            dest_scanln,src,src_pos,src_size,rect_size,pal,scale);
}

/**
 * Gives texture coords for given texture index.
 * @param texture_pos Destination point for storing coordinates.
 * @param cubes The CUBES_DATA structure.
 * @param textr_idx Index of the texture.
 * @param anim_frame Number of the animation frame.
 * @return Returns ERR_NONE on success, and texture coords in texture_pos parameter.
 */
short texture_index_to_texture_pos(struct IPOINT_2D *texture_pos,
    const struct CUBES_DATA *cubes,unsigned short textr_idx,unsigned int anim_frame)
{
    short result=ERR_NONE;
    /* Checking if the index is animated */
    if (textr_idx>=TEXTURE_COUNT_X*TEXTURE_COUNT_Y)
    {
        textr_idx-=TEXTURE_COUNT_X*TEXTURE_COUNT_Y;
        if (textr_idx<cubes->anitxcount)
        {
            int frame;
            if ((textr_idx<12)||(textr_idx>37))
            {
                frame=anim_frame+mdrand_g8(8);
            } else
            {
                frame=anim_frame;
                mdrand_g8_waste(1);
            }
            textr_idx=cubes->anitx[textr_idx].data[frame%8];
        } else
        {
            textr_idx=1;
            mdrand_g8_waste(1);
            result=ERR_DRAW_BADTXTR;
        }
    } else
    {
        mdrand_g8_waste(1);
    }
    texture_pos->x=(textr_idx&7)*TEXTURE_SIZE_X;
    texture_pos->y=((textr_idx>>3)&127)*TEXTURE_SIZE_Y;
    return result;
}

/**
 * Gives texture coords for top of the cube with given index.
 * @param texture_pos Destination point for storing coordinates.
 * @param cubes The CUBES_DATA structure.
 * @param cube_idx Index of the source cube.
 * @param anim Number of the animation frame.
 * @return Returns ERR_NONE on success, and texture coords for top of the cube
 *     in texture_pos parameter.
 */
short get_top_texture_pos(struct IPOINT_2D *texture_pos,
    const struct CUBES_DATA *cubes,unsigned short cube_idx,unsigned int anim)
{
    /* Retrieving texture top index */
    unsigned int textr_top;
    short result=ERR_NONE;
    if (cube_idx<cubes->count)
    {
        textr_top=cubes->data[cube_idx].t;
    } else
    {
        textr_top=1;
    }
    result=texture_index_to_texture_pos(texture_pos,cubes,textr_top,anim);
    if (textr_top==1)
        return ERR_DRAW_BADCUBE;
    return result;
}

/**
 * Draws given LEVEL on given buffer, using graphics and options from MAPDRAW_DATA.
 * @param dest The destination buffer.
 * @param lvl Source level to draw.
 * @param draw_data Graphics textures, sprites and options.
 * @param anim Number of the animation frame.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_map_on_buffer(char *dest,const struct LEVEL *lvl,
    struct MAPDRAW_DATA *draw_data,unsigned int anim)
{
    /*message_log("  draw_map_on_buffer: Starting");*/
    struct IPOINT_2D texture_size={TEXTURE_SIZE_X*TEXTURE_COUNT_X,TEXTURE_SIZE_Y*TEXTURE_COUNT_Y};
    struct IPOINT_2D single_txtr_size={TEXTURE_SIZE_X,TEXTURE_SIZE_Y};
    struct IPOINT_2D dest_size={draw_data->end.x-draw_data->start.x+1,draw_data->end.y-draw_data->start.y+1};
    struct IPOINT_2D texture_pos;
    struct IPOINT_2D dest_pos;
    struct IPOINT_2D scale={1<<(draw_data->rescale),1<<(draw_data->rescale)};
    struct IPOINT_2D scaled_txtr_size={TEXTURE_SIZE_X>>(draw_data->rescale),TEXTURE_SIZE_Y>>(draw_data->rescale)};
    /* Finding start/end of the drawing area */
    struct IPOINT_2D start;
    start.x = draw_data->start.x/scaled_txtr_size.x;
    start.y = draw_data->start.y/scaled_txtr_size.y;
    struct IPOINT_2D end;
    end.x = draw_data->end.x/scaled_txtr_size.x + ((draw_data->end.x%scaled_txtr_size.x)>0);
    end.y = draw_data->end.y/scaled_txtr_size.y + ((draw_data->end.y%scaled_txtr_size.y)>0);
    struct IPOINT_2D stile_count={end.x-start.x-1,end.y-start.y-1};
    /* Drawing subtiles */
    int i,j;
    j=0;
    {
      dest_pos.y=-(draw_data->start.y%scaled_txtr_size.y);
      for (i=0; i<=stile_count.x; i++)
      {
          unsigned short cube_idx;
          unsigned char *clmentry;
          mdrand_setpos(draw_data,start.x+i,start.y);
          clmentry=get_subtile_column(lvl,start.x+i,start.y);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
          draw_texture_on_buffer(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
      }
    }
    for (j=1; j<stile_count.y; j++)
    {
      unsigned short cube_idx;
      unsigned char *clmentry;
      dest_pos.y=j*(scaled_txtr_size.y)-(draw_data->start.y%scaled_txtr_size.y);
      i=0;
      {
          mdrand_setpos(draw_data,start.x,start.y+j);
          clmentry=get_subtile_column(lvl,start.x,start.y+j);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          dest_pos.x=-(draw_data->start.x%scaled_txtr_size.x);
          draw_texture_on_buffer(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
      }
      for (i=1; i<stile_count.x; i++)
      {
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          clmentry=get_subtile_column(lvl,start.x+i,start.y+j);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
          draw_texture_on_buffer_unsafe(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
      }
      { /* i=stile_count.x */
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          clmentry=get_subtile_column(lvl,start.x+i,start.y+j);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
          draw_texture_on_buffer(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
      }
    }
    { /* j=stile_count.y */
      dest_pos.y=j*(scaled_txtr_size.y)-(draw_data->start.y%scaled_txtr_size.y);
      for (i=0; i<=stile_count.x; i++)
      {
          unsigned short cube_idx;
          unsigned char *clmentry;
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          clmentry=get_subtile_column(lvl,start.x+i,start.y+j);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
          draw_texture_on_buffer(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
      }
    }
    /* Colirizing some slabs */
    if (draw_data->ownerpal!=NULL)
      for (j=0; j<end.y-start.y; j++)
      {
      dest_pos.y=j*(scaled_txtr_size.y)-(draw_data->start.y%scaled_txtr_size.y);
        for (i=0; i<end.x-start.x; i++)
        {
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
          unsigned short slab=get_tile_slab(lvl,(start.x+i)/MAP_SUBNUM_X,(start.y+j)/MAP_SUBNUM_Y);
          unsigned char owner=get_subtl_owner(lvl,start.x+i,start.y+j);
          if (slab==SLAB_TYPE_GOLD)
          {
            if (draw_data->intnspal!=NULL)
              draw_rect_mul_on_buffer(dest,dest_pos,dest_size,draw_data->dest_scanln,
                  single_txtr_size,&(draw_data->intnspal[1]),scale);
          } else
          if (slab==SLAB_TYPE_GEMS)
          {
            if (draw_data->intnspal!=NULL)
              draw_rect_mul_on_buffer(dest,dest_pos,dest_size,draw_data->dest_scanln,
                  single_txtr_size,&(draw_data->intnspal[0]),scale);
          } else
          {
            draw_rect_mul_on_buffer(dest,dest_pos,dest_size,draw_data->dest_scanln,
                single_txtr_size,&(draw_data->ownerpal[owner%6]),scale);
          }
        }
      }
      /*message_log("  draw_map_on_buffer: Finished");*/
    return ERR_NONE;
}

/**
 * Draws given LEVEL on given buffer, using graphics and options from MAPDRAW_DATA.
 * Fast version - but a little less quality on rescaling.
 * @param dest The destination buffer.
 * @param lvl Source level to draw.
 * @param draw_data Graphics textures, sprites and options.
 * @param anim Number of the animation frame.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_map_on_buffer_fast(char *dest,const struct LEVEL *lvl,
    struct MAPDRAW_DATA *draw_data,unsigned int anim)
{
    /*message_log("  draw_map_on_buffer_fast: Starting");*/
    struct IPOINT_2D texture_size={TEXTURE_SIZE_X*TEXTURE_COUNT_X,TEXTURE_SIZE_Y*TEXTURE_COUNT_Y};
    struct IPOINT_2D single_txtr_size={TEXTURE_SIZE_X,TEXTURE_SIZE_Y};
    struct IPOINT_2D dest_size={draw_data->end.x-draw_data->start.x+1,draw_data->end.y-draw_data->start.y+1};
    struct IPOINT_2D texture_pos;
    struct IPOINT_2D dest_pos;
    struct IPOINT_2D scale={1<<(draw_data->rescale),1<<(draw_data->rescale)};
    struct IPOINT_2D scaled_txtr_size={TEXTURE_SIZE_X>>(draw_data->rescale),TEXTURE_SIZE_Y>>(draw_data->rescale)};
    /* Finding start/end of the drawing area */
    struct IPOINT_2D start;
    start.x = draw_data->start.x/scaled_txtr_size.x;
    start.y = draw_data->start.y/scaled_txtr_size.y;
    struct IPOINT_2D end;
    end.x = draw_data->end.x/scaled_txtr_size.x + ((draw_data->end.x%scaled_txtr_size.x)>0);
    end.y = draw_data->end.y/scaled_txtr_size.y + ((draw_data->end.y%scaled_txtr_size.y)>0);
    int i,j;
    struct IPOINT_2D tile_count={end.x-start.x-1,end.y-start.y-1};
    dest_pos.y=-(draw_data->start.y%scaled_txtr_size.y);

    j=0;
    { /* j=0 loop - partial slab */
      dest_pos.x=-(draw_data->start.x%scaled_txtr_size.x);
      for (i=0; i<=tile_count.x; i++)
      {
          unsigned short cube_idx;
          unsigned char *clmentry;
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          clmentry=get_subtile_column(lvl,start.x+i,start.y+j);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          draw_texture_on_buffer_fast(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
          dest_pos.x+=scaled_txtr_size.x;
      }
      dest_pos.y+=scaled_txtr_size.y;
    }

    for (j=1; j<tile_count.y; j++)
    {
      dest_pos.x=-(draw_data->start.x%scaled_txtr_size.x);
      i=0;
      { /* i=0 loop - partial slab */
          unsigned short cube_idx;
          unsigned char *clmentry;
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          clmentry=get_subtile_column(lvl,start.x+i,start.y+j);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          draw_texture_on_buffer_fast(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
          dest_pos.x+=scaled_txtr_size.x;
      }
      for (i=1; i<tile_count.x; i++)
      {
          unsigned short cube_idx;
          unsigned char *clmentry;
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          clmentry=get_subtile_column(lvl,start.x+i,start.y+j);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          draw_texture_on_buffer_fast_unsafe(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
          dest_pos.x+=scaled_txtr_size.x;
      }
      { /* i=tile_count.x loop - partial slab */
          unsigned short cube_idx;
          unsigned char *clmentry;
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          clmentry=get_subtile_column(lvl,start.x+i,start.y+j);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          draw_texture_on_buffer_fast(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
          dest_pos.x+=scaled_txtr_size.x;
      }
      dest_pos.y+=scaled_txtr_size.y;
    }
    { /* j=tile_count.y loop - partial slab */
      dest_pos.x=-(draw_data->start.x%scaled_txtr_size.x);
      for (i=0; i<=tile_count.x; i++)
      {
          unsigned short cube_idx;
          unsigned char *clmentry;
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          clmentry=get_subtile_column(lvl,start.x+i,start.y+j);
          cube_idx=get_clm_entry_topcube(clmentry);
          if (cube_idx>0)
              get_top_texture_pos(&texture_pos,draw_data->cubes,cube_idx,anim);
          else
              texture_index_to_texture_pos(&texture_pos,draw_data->cubes,
                  get_clm_entry_base(clmentry),anim);
          draw_texture_on_buffer_fast(dest,dest_pos,dest_size,draw_data->dest_scanln,draw_data->texture,
              texture_pos,texture_size,single_txtr_size,draw_data->palette,scale);
          dest_pos.x+=scaled_txtr_size.x;
      }
      dest_pos.y+=scaled_txtr_size.y;
    }
    /* message_log("  draw_map_on_buffer_fast: Finished"); */
    return ERR_NONE;
}

/**
 * Gives radius to draw object circle for unranged objects.
 * @param scaled_txtr_size Scaled size of one texture (one subtile).
 * @return Returns circle radius as integer value.
 */
unsigned int get_objcircle_std_radius(const struct IPOINT_2D scaled_txtr_size)
{
    unsigned int radius;
    radius=min(scaled_txtr_size.x,scaled_txtr_size.y);
    if (radius>31)
        radius=(radius>>3);
    else
    if (radius>15)
        radius=(radius>>3)+(radius>>4);
    else
    if (radius>3)
        radius=2;
    else
        radius=1;
    return radius;
}

/**
 * Gives radius to draw object circle for ranged objects.
 * @param scaled_txtr_size Scaled size of one texture (one subtile).
 * @param obj_range Range of the object.
 * @param secscale Second scale, for additional scaling.
 * @return Returns circle radius as integer value.
 */
unsigned int get_objcircle_ranged_radius(const struct IPOINT_2D scaled_txtr_size,
    const unsigned int obj_range,const int secscale)
{
    unsigned int radius;
    radius=min(scaled_txtr_size.x,scaled_txtr_size.y);
    radius=(obj_range*radius)>>(8+secscale);
    return radius;
}


/**
 * Draws things from given LEVEL on given buffer, using graphics and options from MAPDRAW_DATA.
 * @param dest The destination buffer.
 * @param lvl Source level to draw things from.
 * @param draw_data Graphics textures, sprites and options.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_things_on_buffer(char *dest,const struct LEVEL *lvl,struct MAPDRAW_DATA *draw_data)
{
    /*message_log("  draw_things_on_buffer: Starting");*/
    struct IPOINT_2D scaled_txtr_size={TEXTURE_SIZE_X>>(draw_data->rescale),TEXTURE_SIZE_Y>>(draw_data->rescale)};
    /* Finding start/end of the drawing area */
    struct IPOINT_2D start;
    start.x = draw_data->start.x/scaled_txtr_size.x;
    start.y = draw_data->start.y/scaled_txtr_size.y;
    struct IPOINT_2D end;
    end.x = draw_data->end.x/scaled_txtr_size.x + ((draw_data->end.x%scaled_txtr_size.x)>0);
    end.y = draw_data->end.y/scaled_txtr_size.y + ((draw_data->end.y%scaled_txtr_size.y)>0);

    /*struct IPOINT_2D dest_scaled_size={(end.x-start.x)*scaled_txtr_size.x,(end.y-start.y)*scaled_txtr_size.y};*/
    struct IPOINT_2D dest_scaled_size={(draw_data->end.x-draw_data->start.x+1),(draw_data->end.y-draw_data->start.y+1)};
    struct IPOINT_2D dest_pos;
    /* Looping through coords and placing things - first pass, the background things */
    int i,j,k;
    for (j=0; j<end.y-start.y; j++)
    {
        for (i=0; i<end.x-start.x; i++)
        {
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          int last_obj=get_thing_subnums(lvl,start.x+i,start.y+j)-1;
          for (k=last_obj; k>=0; k--)
          {
            unsigned char *thing=get_thing(lvl,start.x+i,start.y+j,k);
            int spr_idx=-1;
            if (is_gold(thing))
            {
              /* Show only some of the gold on large scaling */
              if ((draw_data->rescale<4)||(mdrand_g8(7)==0))
                spr_idx=510;
            } else
            if (is_food(thing))
            {
              spr_idx=313;
            } else
            if (is_spellbook(thing))
            {
              spr_idx=60;
            } else
            if (is_trainpost(thing))
            {
              spr_idx=66;
            }
            dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
            dest_pos.x+=((unsigned int)get_thing_subtpos_x(thing)*scaled_txtr_size.x)>>8;
            dest_pos.y=j*(scaled_txtr_size.y)-(draw_data->start.y%scaled_txtr_size.y);
            dest_pos.y+=((unsigned int)get_thing_subtpos_y(thing)*scaled_txtr_size.y)>>8;
            if ((spr_idx>=0)&&(spr_idx<(draw_data->images->count)))
            {
              struct IMAGEITEM *item=&(draw_data->images->items[spr_idx]);
              place_sprite_cntr_on_buf_rgb(dest,dest_pos,dest_scaled_size,
                  draw_data->dest_scanln,draw_data->palette,item);
            }
          }
        }
    }
    /* Second pass - foreground things */
    for (j=0; j<end.y-start.y; j++)
    {
        for (i=0; i<end.x-start.x; i++)
        {
          mdrand_setpos(draw_data,start.x+i,start.y+j);
          int last_obj=get_thing_subnums(lvl,start.x+i,start.y+j)-1;
          for (k=last_obj; k>=0; k--)
          {
            unsigned char *thing=get_thing(lvl,start.x+i,start.y+j,k);
            int spr_idx=-1;
            if (is_creature(thing))
            {
              unsigned short subtp=get_thing_subtype(thing);
              if (subtp<=CREATR_SUBTP_TENTCL)
                spr_idx=175+(2*subtp);
              else
              if (subtp==CREATR_SUBTP_ORC)
                spr_idx=495;
              else
                spr_idx=497;
            } else
            if (is_dngspecbox(thing))
            {
              spr_idx=163;
            }
            dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
            dest_pos.x+=((unsigned int)get_thing_subtpos_x(thing)*scaled_txtr_size.x)>>8;
            dest_pos.y=j*(scaled_txtr_size.y)-(draw_data->start.y%scaled_txtr_size.y);
            dest_pos.y+=((unsigned int)get_thing_subtpos_y(thing)*scaled_txtr_size.y)>>8;
            if ((spr_idx>=0)&&(spr_idx<(draw_data->images->count)))
            {
              struct IMAGEITEM *item=&(draw_data->images->items[spr_idx]);
              place_sprite_cntr_on_buf_rgb(dest,dest_pos,dest_scaled_size,
                  draw_data->dest_scanln,draw_data->palette,item);
            }
          }
        }
    }

  /* Third pass - thing circles */
  if (draw_data->tngflags&TNGFLG_SHOW_CIRCLES)
  {
    mdrand_setpos(draw_data,0,0);
    struct PALETTE_ENTRY *bcolor;
    struct PALETTE_ENTRY *fcolor;
    struct PALETTE_ENTRY ecolor={0,0,0,0};
    int tngradius=get_objcircle_std_radius(scaled_txtr_size);
    for (j=0; j<end.y-start.y; j++)
    {
        for (i=0; i<end.x-start.x; i++)
        {
          unsigned char *obj;
          int radius;
          int last_obj;
          last_obj=get_thing_subnums(lvl,start.x+i,start.y+j)-1;
          for (k=last_obj; k>=0; k--)
          {
            obj=get_thing(lvl,start.x+i,start.y+j,k);
            unsigned char type_idx=get_thing_type(obj);
            bcolor=&thingcircle_palette_std[type_idx%THINGCIRCLE_PALETTE_SIZE];
            dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
            dest_pos.x+=((unsigned int)get_thing_subtpos_x(obj)*scaled_txtr_size.x)>>8;
            dest_pos.y=j*(scaled_txtr_size.y)-(draw_data->start.y%scaled_txtr_size.y);
            dest_pos.y+=((unsigned int)get_thing_subtpos_y(obj)*scaled_txtr_size.y)>>8;
            if (type_idx==THING_TYPE_EFFECTGEN)
            {
              fcolor=&thingcircle_palette_weak[type_idx%THINGCIRCLE_PALETTE_SIZE];
              radius=get_objcircle_ranged_radius(scaled_txtr_size,
                  get_thing_range_adv(obj),0);
              draw_circle_mul(dest,dest_pos,dest_scaled_size,
                  draw_data->dest_scanln,bcolor,fcolor,radius);
            } else
            {
              draw_circle_fill(dest,dest_pos,dest_scaled_size,
                  draw_data->dest_scanln,bcolor,&ecolor,tngradius);
            }
          }
          last_obj=get_stlight_subnums(lvl,start.x+i,start.y+j)-1;
          for (k=last_obj; k>=0; k--)
          {
            obj=get_stlight(lvl,start.x+i,start.y+j,k);
            bcolor=&thingcircle_palette_std[THING_TYPE_ITEM];
            fcolor=&thingcircle_palette_weak[THING_TYPE_ITEM];
            dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
            dest_pos.x+=((unsigned int)get_stlight_subtpos_x(obj)*scaled_txtr_size.x)>>8;
            dest_pos.y=j*(scaled_txtr_size.y)-(draw_data->start.y%scaled_txtr_size.y);
            dest_pos.y+=((unsigned int)get_stlight_subtpos_y(obj)*scaled_txtr_size.y)>>8;
            radius=get_objcircle_ranged_radius(scaled_txtr_size,
                get_stlight_range_adv(obj),1);
            draw_circle_mul(dest,dest_pos,dest_scaled_size,
                draw_data->dest_scanln,bcolor,fcolor,radius);
          }
          last_obj=get_actnpt_subnums(lvl,start.x+i,start.y+j)-1;
          for (k=last_obj; k>=0; k--)
          {
            obj=get_actnpt(lvl,start.x+i,start.y+j,k);
            bcolor=&thingcircle_palette_std[THING_TYPE_DOOR];
            fcolor=&thingcircle_palette_weak[THING_TYPE_DOOR];
            dest_pos.x=i*(scaled_txtr_size.x)-(draw_data->start.x%scaled_txtr_size.x);
            dest_pos.x+=((unsigned int)get_actnpt_subtpos_x(obj)*scaled_txtr_size.x)>>8;
            dest_pos.y=j*(scaled_txtr_size.y)-(draw_data->start.y%scaled_txtr_size.y);
            dest_pos.y+=((unsigned int)get_actnpt_subtpos_y(obj)*scaled_txtr_size.y)>>8;
            radius=get_objcircle_ranged_radius(scaled_txtr_size,
                get_actnpt_range_adv(obj),0);
            draw_circle_mul(dest,dest_pos,dest_scaled_size,
                draw_data->dest_scanln,bcolor,fcolor,radius);
          }
        }
    }
  }
  /*message_log("  draw_things_on_buffer: Finished");*/
  return ERR_NONE;
}

/**
 * Draws text on given buffer, using graphics and options from MAPDRAW_DATA.
 * @param dest The destination buffer.
 * @param px,py Start point for the dawing in dest buffer.
 * @param text Source text message.
 * @param draw_data Graphics textures, sprites and options.
 * @param font Font selector.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short draw_text_on_buffer(char *dest,const int px,const int py,
    const char *text,struct MAPDRAW_DATA *draw_data,short font)
{
    struct IPOINT_2D dest_scaled_size={(draw_data->end.x-draw_data->start.x+1),(draw_data->end.y-draw_data->start.y+1)};
    struct IPOINT_2D dest_pos={px,py};
    int i;
    struct IMAGELIST *fontimgs;
    if (font==0)
        fontimgs=draw_data->font0;
    else
        fontimgs=draw_data->font1;
    if ((text==NULL)||(fontimgs==NULL))
        return ERR_INTERNAL;
    for (i=0;text[i]!='\0';i++)
    {
        short spr_idx=(short)((unsigned char)text[i]) - 32;
        if ((spr_idx>=0)&&(spr_idx<(fontimgs->count)))
        {
              struct IMAGEITEM *item=&(fontimgs->items[spr_idx]);
              place_sprite_cntr_on_buf_rgb(dest,dest_pos,dest_scaled_size,
                  draw_data->dest_scanln,draw_data->palette,item);
              dest_pos.x+=item->width;
        }
    }
    /*message_log("  draw_text_on_buffer: Finished");*/
    return ERR_NONE;
}

/**
 * Writes RGB colour bitmap into file with given name.
 * @param fname Destination file name.
 * @param data Bitmap data buffer.
 * @param size Bitmap width and height.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_bitmap_rgb(const char *fname,const unsigned char *data,const struct IPOINT_2D size)
{
  message_log("  write_bitmap_rgb: Starting");
  if ((fname==NULL)||(data==NULL))
  {
      message_error("Internal error - null poiner detected in write_bitmap_rgb");
      return 2;
  }

  FILE *out;
  out = fopen (fname, "wb");
  if (out==NULL)
  {
      message_error("Can't open \"%s\" for writing", fname);
      return 1;
  }

  short result;
  result=write_bmp_fp_24b(out,size.x,size.y,data);
  fclose(out);
  message_log("  write_bitmap_rgb: Finished");
  return result;
}

/**
 * Sets the drawing rectangle in MAPDRAW_DATA structure.
 * @see set_draw_data_rect_ip
 * @param draw_data Destination structure.
 * @param startx Starting coordinate.
 * @param starty Starting coordinate.
 * @param endx Ending coordinate.
 * @param endy Ending coordinate.
 * @param scanline Destination buffer scanline length.
 * @param rescale Scale factor.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short set_draw_data_rect(struct MAPDRAW_DATA *draw_data,
    const int startx,const int starty,const int endx,const int endy,
    const unsigned int scanline,const short rescale)
{
    draw_data->start.x=startx;
    draw_data->start.y=starty;
    draw_data->end.x=endx;
    draw_data->end.y=endy;
    if (rescale > 3)
    {
      draw_data->ownerpal=owned_area_palette_std;
      draw_data->intnspal=intense_slab_palette_std;
    } else
    if (rescale > 2)
    {
      draw_data->ownerpal=owned_area_palette_weak;
      draw_data->intnspal=intense_slab_palette_weak;
    } else
    {
      draw_data->ownerpal=NULL;
      draw_data->intnspal=NULL;
    }
    draw_data->rescale=rescale;
    draw_data->dest_scanln=scanline;
    return ERR_NONE;
}

/**
 * Sets the drawing rectangle in MAPDRAW_DATA structure.
 * Uses IPOINT_2D structs instead of single points.
 * @see set_draw_data_rect
 * @param draw_data Destination structure.
 * @param start Starting coordinates.
 * @param end Ending coordinates.
 * @param scanline Destination buffer scanline length.
 * @param rescale Scale factor.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short set_draw_data_rect_ip(struct MAPDRAW_DATA *draw_data,
    const struct IPOINT_2D start,const struct IPOINT_2D end,
    const unsigned int scanline,const short rescale)
{
    return set_draw_data_rect(draw_data,start.x,start.y,end.x,end.y,scanline,rescale);
}

/**
 * Sets the drawing rectangle in MAPDRAW_DATA structure.
 * Sets a scanline length to be multiplication of 4 - needed in some cases.
 * @see set_draw_data_rect
 * @param draw_data Destination structure.
 * @param startx Starting coordinate.
 * @param starty Starting coordinate.
 * @param endx Ending coordinate.
 * @param endy Ending coordinate.
 * @param dest_sizex Destination buffer line size, in pixels.
 * @param rescale Scale factor.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short set_draw_data_rect_sl4(struct MAPDRAW_DATA *draw_data,
    const int startx,const int starty,const int endx,const int endy,
    const short dest_sizex,const short rescale)
{
    unsigned int scanln=(3*dest_sizex);
    if ((scanln%4)!=0) scanln+=4-(scanln%4);
    return set_draw_data_rect(draw_data,startx,starty,endx,endy,scanln,rescale);
}

/**
 * Allocates and fills the MAPDRAW_DATA structure.
 * Loads all data files needed to draw the map.
 * Sets drawing rectangle from (0,0) to bmp_size.
 * @param draw_data Destination structure.
 * @param opts Drawing options.
 * @param subtl Size of the map, in subtiles.
 * @param bmp_size Ending coords of the drawing rectangle.
 * @param textr_idx Texture file index.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_draw_data(struct MAPDRAW_DATA **draw_data,const struct MAPDRAW_OPTIONS *opts,
    const struct UPOINT_2D *subtl,const struct IPOINT_2D bmp_size,int textr_idx)
{
    short result;
    (*draw_data) = malloc(sizeof(struct MAPDRAW_DATA));
    if ((*draw_data) == NULL)
    {
        message_error("load_draw_data: Cannot allocate draw_data memory.");
        return 2;
    }
    (*draw_data)->subsize.x=subtl->x;
    (*draw_data)->subsize.y=subtl->y;
    (*draw_data)->tngflags=opts->tngflags;
    unsigned int total_subtiles=(*draw_data)->subsize.x*(*draw_data)->subsize.y;
    /* Initializing draw_data values */
    (*draw_data)->cubes=malloc(sizeof(struct CUBES_DATA));
    (*draw_data)->palette=malloc(256*sizeof(struct PALETTE_ENTRY));
    (*draw_data)->images=malloc(sizeof(struct IMAGELIST));
    (*draw_data)->rand_size=total_subtiles*sizeof(int);
    (*draw_data)->rand_pool=malloc((*draw_data)->rand_size);
    if ((*draw_data)->cubes != NULL)
    {
        (*draw_data)->cubes->count=0;
        (*draw_data)->cubes->data=NULL;
    }
    if (opts->bmfonts&BMFONT_LOAD_SMALL)
        (*draw_data)->font0=malloc(sizeof(struct IMAGELIST));
    else
        (*draw_data)->font0=NULL;
    if (opts->bmfonts&BMFONT_LOAD_LARGE)
        (*draw_data)->font1=malloc(sizeof(struct IMAGELIST));
    else
        (*draw_data)->font1=NULL;
    (*draw_data)->texture=NULL;
    (*draw_data)->ownerpal=NULL;
    (*draw_data)->intnspal=NULL;
    if (((*draw_data)->cubes==NULL) || ((*draw_data)->palette==NULL) ||
        (((*draw_data)->font0==NULL)&&(opts->bmfonts&BMFONT_LOAD_SMALL)) ||
        (((*draw_data)->font1==NULL)&&(opts->bmfonts&BMFONT_LOAD_LARGE)) ||
        ((*draw_data)->images==NULL) || ((*draw_data)->rand_pool==NULL) )
    {
        message_error("load_draw_data: Out of memory.");
        free_draw_data(*draw_data);
        (*draw_data)=NULL;
        return 3;
    }
    /* Preparing random pool */
    int i;
    for (i=0;i<total_subtiles;i++)
    {
        int *rnd_ints=(int *)(*draw_data)->rand_pool;
        rnd_ints[i]=rand();
    }
    (*draw_data)->rand_count=0;
    /* Setting map drawing rectangle */
    set_draw_data_rect(*draw_data,0,0,bmp_size.x-1,bmp_size.y-1,bmp_size.x*3,(opts->rescale));
    /* Loading files needed to draw */
    char *fnames;
    result=true;
    if (result)
    {
      message_log(" load_draw_data: Loading \"%s\"",palette_fname);
      fnames=NULL;
      result = format_data_fname(&fnames,opts->data_path,palette_fname);
      if (result)
        result = (load_palette((*draw_data)->palette,fnames)==ERR_NONE);
      if (!result)
          message_error("Error when loading file \"%s\"",fnames);
      free(fnames);
    }
    if (result)
    {
      message_log(" load_draw_data: Loading \"%s\"",cube_fname);
      fnames=NULL;
      format_data_fname(&fnames,opts->data_path,cube_fname);
      result = (load_cubedata((*draw_data)->cubes,fnames)==ERR_NONE);
      if (!result)
          message_error("Error when loading file \"%s\"",fnames);
      free(fnames);
    }
    if (result)
    {
      message_log(" load_draw_data: Loading \"%s\"",tmapanim_fname);
      fnames=NULL;
      format_data_fname(&fnames,opts->data_path,tmapanim_fname);
      result = (load_textureanim((*draw_data)->cubes,fnames)==ERR_NONE);
      if (!result)
          message_error("Error when loading file \"%s\"",fnames);
      free(fnames);
    }
    if (result)
    {
      message_log(" load_draw_data: Loading texture");
      result = (change_draw_data_texture(*draw_data,opts,textr_idx)==ERR_NONE);
    }
    /* Reading DAT,TAB and extracting images */
    if (result)
    {
      message_log(" load_draw_data: Loading gui2-0 icons");
      char *tabfname;
      fnames=NULL;
      tabfname=NULL;
      int large_tngicons=((opts->rescale)<3);
      format_data_fname(&fnames,opts->data_path,"gui%d-%d-%d.dat",2,0,large_tngicons);
      format_data_fname(&tabfname,opts->data_path,"gui%d-%d-%d.tab",2,0,large_tngicons);
      result = (create_images_dattab_idx((*draw_data)->images,fnames,tabfname,0)==ERR_NONE);
      if (!result)
          message_error("Error when loading dat/tab pair \"%s\"",fnames);
      free(fnames);
      free(tabfname);
    }
    /* Reading font0 DAT,TAB and extracting images */
    if ((result)&&(opts->bmfonts&BMFONT_LOAD_SMALL))
    {
      message_log(" load_draw_data: Loading small font");
      char *tabfname;
      fnames=NULL;
      tabfname=NULL;
      /*int large_tngicons=((opts->rescale)<3);*/
      format_data_fname(&fnames,opts->data_path,"font%d-%d.dat",2,0);
      format_data_fname(&tabfname,opts->data_path,"font%d-%d.tab",2,0);
      result = (create_images_dattab_idx((*draw_data)->font0,fnames,tabfname,0)==ERR_NONE);
      if (!result)
          message_error("Error when loading dat/tab pair \"%s\"",fnames);
      free(fnames);
      free(tabfname);
    }
    /* Reading font1 DAT,TAB and extracting images */
    if ((result)&&(opts->bmfonts&BMFONT_LOAD_LARGE))
    {
      message_log(" load_draw_data: Loading large font");
      char *tabfname;
      fnames=NULL;
      tabfname=NULL;
      /*int large_tngicons=((opts->rescale)<3);*/
      format_data_fname(&fnames,opts->data_path,"font%d-%d.dat",2,1);
      format_data_fname(&tabfname,opts->data_path,"font%d-%d.tab",2,1);
      result = (create_images_dattab_idx((*draw_data)->font1,fnames,tabfname,0)==ERR_NONE);
      if (!result)
          message_error("Error when loading dat/tab pair \"%s\"",fnames);
      free(fnames);
      free(tabfname);
    }
    /* Preparing constant arrays */
    if (result)
    {
      message_log(" load_draw_data: Preparing constant arrays");
      for(i=0;i<SIN_ACOS_SIZE;i++)                 /* create the sin(arccos(x)) table. */
      {
        (*draw_data)->sin_acos[i]=sin(acos(((float)i)/SIN_ACOS_SIZE))*0x10000L;
      }
    }
    if (!result)
    {
        free_draw_data(*draw_data);
        (*draw_data)=NULL;
        return 1;
    }
    return ERR_NONE;
}

/**
 * Changes loaded texture in the MAPDRAW_DATA structure.
 * Loads the new texture file from disk into draw_data.
 * @param draw_data Destination structure.
 * @param opts Drawing options.
 * @param textr_idx New texture file index.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short change_draw_data_texture(struct MAPDRAW_DATA *draw_data,
    const struct MAPDRAW_OPTIONS *opts,const int textr_idx)
{
  char *fname;
  short result;
  format_data_fname(&fname,opts->data_path,"tmapa%03d.dat",textr_idx);
  free(draw_data->texture);
  draw_data->texture=NULL;
  result = load_texture(&(draw_data->texture),fname);
  if (result!=ERR_NONE)
      message_error("Error when loading file \"%s\"",fname);
  free(fname);
  return ERR_NONE;
}

/**
 * Frees the MAPDRAW_DATA structure.
 * @param draw_data Destination structure.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short free_draw_data(struct MAPDRAW_DATA *draw_data)
{
  if (draw_data==NULL)
      return ERR_NONE;
  free_dattab_images(draw_data->images);
  free(draw_data->images);
  if (draw_data->cubes != NULL)
      free(draw_data->cubes->data);
  free(draw_data->texture);
  free(draw_data->palette);
  free(draw_data->cubes);
  free(draw_data->rand_pool);
  free(draw_data);
  return ERR_NONE;
}

/**
 * Returns coordinates of a graphics buffer rectangle where specific tile is.
 * @param tstart Tile start (top left) coordinates.
 * @param tdim Tile dimensions. Adding them to tstart will give you
 *     end (bottom right) coordinates.
 * @param draw_data The drawing data structure.
 * @param tx,ty Coords of the tile we want to get graphics rectangle for.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns coordinates in IPOINT_2D structures.
 */
short get_draw_tile_rect(struct IPOINT_2D *tstart,
    struct IPOINT_2D *tdim, const struct MAPDRAW_DATA *draw_data,
    unsigned int tx,unsigned int ty)
{
  if (draw_data==NULL)
      return ERR_INTERNAL;
  tdim->x = (TEXTURE_SIZE_X>>(draw_data->rescale))*MAP_SUBNUM_X;
  tdim->y = (TEXTURE_SIZE_Y>>(draw_data->rescale))*MAP_SUBNUM_Y;
  /* Finding start of the tile area */
  tstart->x = (tdim->x*tx)-draw_data->start.x;
  tstart->y = (tdim->y*ty)-draw_data->start.y;
  return ERR_NONE;
}

/**
 * Returns coordinates of a graphics buffer rectangle where specific subtile is.
 * @param sstart Subtile start (top left) coordinates.
 * @param sdim Subtile dimensions. Adding them to sstart will give you
 *     end (bottom right) coordinates.
 * @param draw_data The drawing data structure.
 * @param sx,sy Coords of the subtile we want to get graphics rectangle for.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns coordinates in IPOINT_2D structures.
 */
short get_draw_subtile_rect(struct IPOINT_2D *sstart,
    struct IPOINT_2D *sdim, const struct MAPDRAW_DATA *draw_data,
    unsigned int sx,unsigned int sy)
{
  if (draw_data==NULL)
      return ERR_INTERNAL;
  sdim->x = (TEXTURE_SIZE_X>>(draw_data->rescale));
  sdim->y = (TEXTURE_SIZE_Y>>(draw_data->rescale));
  /* Finding start of the tile area */
  sstart->x = (sdim->x*sx)-draw_data->start.x;
  sstart->y = (sdim->y*sy)-draw_data->start.y;
  return ERR_NONE;
}

/**
 * Returns subtile coordinates for given graphics map position.
 * @param sx Destination subtile X coordinate.
 * @param sy Destination subtile Y coordinate.
 * @param draw_data The drawing data structure.
 * @param px,py Coords of the graphics map point at which we want subtile number.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns coordinates in (sx,sy).
 */
short get_subtile_from_draw_coords(unsigned int *sx,unsigned int *sy,
    const struct MAPDRAW_DATA *draw_data,
    unsigned int px,unsigned int py)
{
  if (draw_data==NULL)
      return ERR_INTERNAL;
  struct IPOINT_2D scaled_txtr_size=
      {TEXTURE_SIZE_X>>(draw_data->rescale),TEXTURE_SIZE_Y>>(draw_data->rescale)};
  (*sx)=(px+draw_data->start.x)/scaled_txtr_size.x;
  (*sy)=(py+draw_data->start.y)/scaled_txtr_size.y;
  return ERR_NONE;
}

/**
 * Returns subtile coordinates for given graphics map position.
 * Returns exact coords within a subtile.
 * @param subtl Destination subtile coordinates.
 * @param subpos Destination position within the subtile.
 * @param draw_data The drawing data structure.
 * @param px,py Coords of the graphics map point at which we want subtile number.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns coordinates in (sx,sy).
 */
short get_subtile_from_draw_coords_ip(struct IPOINT_2D *subtl,
    struct IPOINT_2D *subpos,const struct MAPDRAW_DATA *draw_data,
    unsigned int px,unsigned int py)
{
  if (draw_data==NULL)
      return ERR_INTERNAL;
  struct IPOINT_2D scaled_txtr_size=
      {TEXTURE_SIZE_X>>(draw_data->rescale),TEXTURE_SIZE_Y>>(draw_data->rescale)};
  struct IPOINT_2D absol_coords=
      {px+draw_data->start.x,py+draw_data->start.y};
  subtl->x = absol_coords.x/scaled_txtr_size.x;
  subtl->y = absol_coords.y/scaled_txtr_size.y;
  subpos->x = (absol_coords.x%scaled_txtr_size.x)*(256/scaled_txtr_size.x);
  subpos->y = (absol_coords.y%scaled_txtr_size.y)*(256/scaled_txtr_size.y);
  return ERR_NONE;
}

/**
 * Returns subtile coordinates for given graphics map position.
 * Returns exact coords within a subtile plus subtile number shifted by 8 bits.
 * @param ssx,ssy Destination coordinates: subtile<<8+subtile_pos.
 * @param draw_data The drawing data structure.
 * @param px,py Coords of the graphics map point at which we want subtile number.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns coordinates in (sx,sy).
 */
short get_subtile_from_draw_coords_adv(unsigned int *ssx,unsigned int *ssy,
    const struct MAPDRAW_DATA *draw_data,unsigned int px,unsigned int py)
{
  if (draw_data==NULL)
      return ERR_INTERNAL;
  struct IPOINT_2D scaled_txtr_size=
      {TEXTURE_SIZE_X>>(draw_data->rescale),TEXTURE_SIZE_Y>>(draw_data->rescale)};
  struct IPOINT_2D absol_coords=
      {px+draw_data->start.x,py+draw_data->start.y};
  *ssx = ((absol_coords.x/scaled_txtr_size.x)<<8) +
      (absol_coords.x%scaled_txtr_size.x)*(256/scaled_txtr_size.x);
  *ssy = ((absol_coords.y/scaled_txtr_size.y)<<8) +
      (absol_coords.y%scaled_txtr_size.y)*(256/scaled_txtr_size.y);
  return ERR_NONE;
}

/**
 * Returns subtile coordinates for given graphics map position.
 * Returns exact coords within a subtile plus subtile number shifted by 8 bits.
 * Fast version - gets scalet texture as parameter, so it don't have to be recomputed.
 * @param ssx,ssy Destination coordinates: subtile<<8+subtile_pos.
 * @param scaled_txtr_size The scaled size of texture (subtile size on graphics buffer).
 * @param start Map drawing starting coords.
 * @param px,py Coords of the graphics map point at which we want subtile number.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns coordinates in (sx,sy).
 */
short get_subtile_from_draw_coords_adv_fast(unsigned int *ssx,unsigned int *ssy,
    const struct IPOINT_2D scaled_txtr_size,const struct IPOINT_2D start,
    unsigned int px,unsigned int py)
{
  struct IPOINT_2D absol_coords=
      {px+start.x,py+start.y};
  *ssx = ((absol_coords.x/scaled_txtr_size.x)<<8) +
      (absol_coords.x%scaled_txtr_size.x)*(256/scaled_txtr_size.x);
  *ssy = ((absol_coords.y/scaled_txtr_size.y)<<8) +
      (absol_coords.y%scaled_txtr_size.y)*(256/scaled_txtr_size.y);
  return ERR_NONE;
}

/**
 * Returns a thing which is at given position in map image buffer.
 * The (sx,sy,num) indices can be used to get the thing data
 * like this: get_thing(lvl,sx,sy,num).
 * @param sx,sy,num Destination indices of thing.
 * @param draw_data The drawing data structure.
 * @param lvl Pointer to the level structure.
 * @param px,py Coords of the graphics map point at which we want the thing.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns indices in (sx,sy,num).
 */
short get_thing_with_circle_at(unsigned int *sx,unsigned int *sy,unsigned int *num,
    const struct MAPDRAW_DATA *draw_data,const struct LEVEL *lvl,
    unsigned int px,unsigned int py)
{
  /*message_log(" get_thing_with_circle_at: Starting with pos (%u,%u)",px,py);*/
  if (draw_data==NULL)
      return ERR_INTERNAL;
  struct IPOINT_2D scaled_txtr_size=
      {TEXTURE_SIZE_X>>(draw_data->rescale),TEXTURE_SIZE_Y>>(draw_data->rescale)};
  short result;
  unsigned int test_ssx,test_ssy;
  result=get_subtile_from_draw_coords_adv_fast(&test_ssx,&test_ssy,
      scaled_txtr_size,draw_data->start,px,py);
  if (result!=ERR_NONE)
  {
      message_error("Cannot convert coordinates");
      return result;
  }
  /*message_log(" get_thing_with_circle_at: Searching for thing at (%u,%u)",test_ssx,test_ssy);*/
  /* Searching for nearest thing */
  unsigned int obj_sx,obj_sy,obj_num;
  long dist;
  dist=get_nearest_thing_idx(lvl,test_ssx,test_ssy,&obj_sx,&obj_sy,&obj_num);
  if (dist<0)
  {
      message_info("No thing near subtile (%u,%u)",test_ssx>>8,test_ssy>>8);
      return ERR_INTERNAL;
  }
  /*message_log(" get_thing_with_circle_at: Thing found");*/
  /**
   * Checking if it is near enough
   * Note: it is quite simplified and assumes scale is same in X and Y direction!
   */
  long radius;
  radius=get_objcircle_std_radius(scaled_txtr_size);
  if (radius<3) radius=3;
  radius = ((unsigned long)radius<<8)/scaled_txtr_size.x;
  /* Equation of a circle */
  if (dist<=radius)
  {
      /*message_log(" get_thing_with_circle_at: Thing mets condition");*/
      *sx=obj_sx;
      *sy=obj_sy;
      *num=obj_num;
      return ERR_NONE;
  }
  /*message_log(" get_thing_with_circle_at: Thing rejected");*/
  return ERR_INTERNAL;
}

/**
 * Returns an object which is at given position in map image buffer.
 * The (sx,sy,z) indices can be used to get the object data.
 * You can identify the object type by calling get_object_type(lvl,sx,sy,z).
 * @param sx,sy,z Destination indices of object.
 * @param draw_data The drawing data structure.
 * @param lvl Pointer to the level structure.
 * @param px,py Coords of the graphics map point at which we want the object.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns indices in (sx,sy,z).
 */
short get_object_with_circle_at(unsigned int *sx,unsigned int *sy,unsigned int *z,
    const struct MAPDRAW_DATA *draw_data,const struct LEVEL *lvl,
    unsigned int px,unsigned int py)
{
  /*message_log(" get_object_with_circle_at: Starting with pos (%u,%u)",px,py);*/
  if (draw_data==NULL)
      return ERR_INTERNAL;
  struct IPOINT_2D scaled_txtr_size=
      {TEXTURE_SIZE_X>>(draw_data->rescale),TEXTURE_SIZE_Y>>(draw_data->rescale)};
  short result;
  unsigned int test_ssx,test_ssy;
  result=get_subtile_from_draw_coords_adv_fast(&test_ssx,&test_ssy,
      scaled_txtr_size,draw_data->start,px,py);
  if (result!=ERR_NONE)
  {
      message_error("Cannot convert coordinates");
      return result;
  }
  /*message_log(" get_object_with_circle_at: Searching for objects at (%u,%u)",test_ssx,test_ssy);*/
  /* Searching for nearest object */
  unsigned int obj_sx,obj_sy,obj_num;
  long dist;
  dist=get_nearest_object_idx(lvl,test_ssx,test_ssy,&obj_sx,&obj_sy,&obj_num);
  if (dist<0)
  {
      message_info("No object near subtile (%u,%u)",test_ssx>>8,test_ssy>>8);
      return ERR_INTERNAL;
  }
  /*message_log(" get_object_with_circle_at: Object found");*/
  /**
   * Checking if it is near enough
   * Note: it is quite simplified and assumes scale is same in X and Y direction!
   */
  long radius;
  radius=get_objcircle_std_radius(scaled_txtr_size);
  if (radius<3) radius=3;
  radius = ((unsigned long)radius<<8)/scaled_txtr_size.x;
  /* Equation of a circle */
  if (dist<=radius)
  {
      /*message_log(" get_object_with_circle_at: Object mets condition");*/
      *sx=obj_sx;
      *sy=obj_sy;
      *z=obj_num;
      return ERR_NONE;
  }
  /*message_log(" get_object_with_circle_at: Object rejected");*/
  return ERR_INTERNAL;
}

/**
 * Returns full dimensions of the level graphics view in current scale.
 * @param gdim Graphics map dimensions. Substracting draw_data->start will
 *     give you dimensions from current start position.
 * @param draw_data The drawing data structure.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns coordinates in IPOINT_2D structures.
 */
short get_full_draw_dimensions(struct IPOINT_2D *gdim,
    const struct MAPDRAW_DATA *draw_data)
{
  if (draw_data==NULL)
      return ERR_INTERNAL;
  struct IPOINT_2D scaled_txtr_size=
      {TEXTURE_SIZE_X>>(draw_data->rescale),TEXTURE_SIZE_Y>>(draw_data->rescale)};
  gdim->x = scaled_txtr_size.x*draw_data->subsize.x;
  gdim->y = scaled_txtr_size.y*draw_data->subsize.y;
  return ERR_NONE;
}

/**
 * Returns full dimensions of the level graphics view in current scale.
 * Requires MAPDRAW_OPTIONS struct and map size instead of MAPDRAW_DATA.
 * @param gdim Graphics map dimensions. Substracting draw_data->start will
 *     give you dimensions from current start position.
 * @param subtl The map size, in subtiles.
 * @param opts The drawing options structure.
 * @return Returns ERR_NONE on success, error code on failure.
 *     On success, returns coordinates in IPOINT_2D structures.
 */
short get_full_draw_dimensions_opt(struct IPOINT_2D *gdim,
    const struct UPOINT_2D *subtl,const struct MAPDRAW_OPTIONS *opts)
{
  if (opts==NULL)
      return ERR_INTERNAL;
  struct IPOINT_2D scaled_txtr_size=
      {TEXTURE_SIZE_X>>(opts->rescale),TEXTURE_SIZE_Y>>(opts->rescale)};
  gdim->x = scaled_txtr_size.x*subtl->x;
  gdim->y = scaled_txtr_size.y*subtl->y;
  return ERR_NONE;
}

/**
 * Generates bitmap representing the current map layout.
 * The result is stored into given file name.
 * Loads all data files every time it's executed.
 * @param bmpfname Output bitmap file name.
 * @param lvl Source level to draw map from.
 * @param opts Drawing options.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short generate_map_bitmap(const char *bmpfname,const struct LEVEL *lvl,
    const struct MAPDRAW_OPTIONS *opts)
{
    message_log(" generate_map_bitmap: Starting");
    short result;
    struct MAPDRAW_DATA *draw_data;
    /* Texture and bitmap size */
    struct IPOINT_2D textr_size={TEXTURE_SIZE_X>>(opts->rescale),TEXTURE_SIZE_Y>>(opts->rescale)};
    struct IPOINT_2D bmp_size;
    /* Settings to draw whole map */
    bmp_size.x=textr_size.x*lvl->subsize.x;
    bmp_size.y=textr_size.y*lvl->subsize.y;
    result=load_draw_data(&draw_data,opts,&(lvl->subsize),bmp_size,(int)(lvl->inf%8));
    if (result!=ERR_NONE)
        return result;
    unsigned char *bitmap=NULL;
    if (result==ERR_NONE)
    {
      bitmap=(unsigned char *)malloc((bmp_size.x*bmp_size.y+1)*3);
      if (bitmap==NULL)
      {
        message_error("generate_map_bitmap: Cannot allocate bitmap memory.");
        free_draw_data(draw_data);
        return 2;
      }
    }
    if (result==ERR_NONE)
    {
      result = draw_map_on_buffer(bitmap,lvl,draw_data,rnd(32768));
      if (result!=ERR_NONE)
          message_error("Error when drawing map on memory buffer");
    }
    if (result==ERR_NONE)
    {
      if ((opts->rescale)<5)
        result = draw_things_on_buffer(bitmap,lvl,draw_data);
      if (result!=ERR_NONE)
          message_error("Error when placing thing sprites on memory buffer");
    }
    if (result==ERR_NONE)
    {
      /*result = write_bitmap_idx(bmpfname, draw_data->palette, bitmap, bmp_size);*/
      result = write_bitmap_rgb(bmpfname,bitmap,bmp_size);
    }
    free(bitmap);
    free_draw_data(draw_data);
    message_log(" generate_map_bitmap: Finished");
    return result;
}

/**
 * Generates bitmap representing the current map layout.
 * Saves it on the filename same as level name, and with BMP extension.
 * @param lvl Source level to draw map from.
 */
short generate_map_bitmap_mapfname(struct LEVEL *lvl)
{
    short result;
    char *bmpfname;
    bmpfname = (char *)malloc(strlen(lvl->fname)+5);
    if (bmpfname==NULL)
    {
        message_error("generate_map_bitmap_mapfname: Cannot allocate memory.");
        return false;
    }
    sprintf (bmpfname, "%s.bmp", lvl->fname);
    lvl->optns.picture.data_path=lvl->optns.data_path;
    result = generate_map_bitmap(bmpfname,lvl,&(lvl->optns.picture));
    if (result==ERR_NONE)
      message_info("Bitmap \"%s\" created.",bmpfname);
    free(bmpfname);
    return result;
}

