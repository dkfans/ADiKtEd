/******************************************************************************/
// xcubtxtr.h - Dungeon Keeper Tools.
/******************************************************************************/
// Author:  Tomasz Lis
// Created: 19 Apr 2008

// Purpose:
//   Header file. Defines exported routines from xcubtxtr.h

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef BULL_XCUBTXTR_H
#define BULL_XCUBTXTR_H

// No error condition need to be same as in read_file (MFILE_OK)
// There should be no more than 32 errors (up to -31)
// values to 23 are reserved by read_file and RNC library
#define XCUTX_FILE_BADDATA  1
#define XCUTX_CANNOT_OPEN   2
#define XCUTX_MALLOC_ERR    3
#define XCUTX_INTERNAL      4

#define SIZEOF_DK_CUBE_REC 18
#define SIZEOF_DK_CUBE_HEADER 4

#define TEXTURE_SIZE_X 32
#define TEXTURE_SIZE_Y 32
#define TEXTURE_COUNT_X 8
#define TEXTURE_COUNT_Y 68

//Animated texture - indices of textures which are swiching
struct CUBE_TXTRANIM {
    unsigned int data[8];
};

//Texture indices for every side of a cube
struct CUBE_TEXTURES {
       //north
    unsigned int n;
       //south
    unsigned int s;
       //west
    unsigned int w;
       //east
    unsigned int e;
       //top
    unsigned int t;
       //bottom
    unsigned int b;
       //unknown (unused?)
    unsigned int u[3];
};

struct CUBES_DATA {
    struct CUBE_TEXTURES *data;
    int count;
    struct CUBE_TXTRANIM *anitx;
    int anitxcount;
};

short alloc_cubedata(struct CUBES_DATA *cubes,long count);
short free_cubedata(struct CUBES_DATA *cubes);

short load_cubedata(struct CUBES_DATA *cubes,const char *fname);
short write_cubedata(struct CUBES_DATA *cubes,const char *fname);

short switch_cubedata_entries(struct CUBES_DATA *cubes,unsigned long idx1,unsigned long idx2);

short load_textureanim(struct CUBES_DATA *cubes,const char *fname);
short load_texture(unsigned char **texture,const char *fname);

#endif
