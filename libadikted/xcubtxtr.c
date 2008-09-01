/******************************************************************************/
// xcubtxtr.c - Dungeon Keeper Tools.
/******************************************************************************/
// Author:  Tomasz Lis
// Created: 19 Apr 2008

// Purpose:
//   Loads and writes Dungeon Keeper cube definition and texture files.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bulcommn.h"
#include "xcubtxtr.h"
#include "memfile.h"

short alloc_cubedata(struct CUBES_DATA *cubes,long count)
{
    cubes->count=count;
    cubes->data=malloc(cubes->count*sizeof(struct CUBE_TEXTURES));
    if (cubes->data==NULL)
    {
        cubes->count=0;
        return XCUTX_MALLOC_ERR;
    }
    return ERR_NONE;
}

short free_cubedata(struct CUBES_DATA *cubes)
{
    free(cubes->data);
    cubes->count=0;
    cubes->data=NULL;
    return ERR_NONE;
}

short load_cubedata(struct CUBES_DATA *cubes,const char *fname)
{
    //Reading file
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    // Checking file size
    if (mem->len<22)
    {
      memfile_free(&mem);
      return XCUTX_FILE_BADDATA;
    }
    unsigned long count=read_int32_le_buf(mem->content+0);
    if ((mem->len!=count*SIZEOF_DK_CUBE_REC+SIZEOF_DK_CUBE_HEADER))
    {
      memfile_free(&mem);
      return XCUTX_FILE_BADDATA;
    }
    //Loading the entries
    if (alloc_cubedata(cubes,count)!=ERR_NONE)
    {
      memfile_free(&mem);
      return XCUTX_MALLOC_ERR;
    }
    int i;
    for (i=0; i<cubes->count; i++)
    {
        unsigned int item_pos=i*SIZEOF_DK_CUBE_REC+SIZEOF_DK_CUBE_HEADER;
        unsigned int val;
        val=read_int16_le_buf(mem->content+item_pos+ 0);
        cubes->data[i].n=val;
        val=read_int16_le_buf(mem->content+item_pos+ 2);
        cubes->data[i].s=val;
        val=read_int16_le_buf(mem->content+item_pos+ 4);
        cubes->data[i].w=val;
        val=read_int16_le_buf(mem->content+item_pos+ 6);
        cubes->data[i].e=val;
        val=read_int16_le_buf(mem->content+item_pos+ 8);
        cubes->data[i].t=val;
        val=read_int16_le_buf(mem->content+item_pos+10);
        cubes->data[i].b=val;
        val=read_int16_le_buf(mem->content+item_pos+12);
        cubes->data[i].u[0]=val;
        val=read_int16_le_buf(mem->content+item_pos+14);
        cubes->data[i].u[1]=val;
        val=read_int16_le_buf(mem->content+item_pos+16);
        cubes->data[i].u[2]=val;
    }
    memfile_free(&mem);
    return ERR_NONE;
}

short write_cubedata(struct CUBES_DATA *cubes,const char *fname)
{
    //Opening file
    FILE *fp;
    int i, k;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return XCUTX_CANT_OPEN;
    //Writing header
    write_int32_le_file(fp,cubes->count);
    for (i=0; i<cubes->count; i++)
    {
        struct CUBE_TEXTURES *data = &(cubes->data[i]);
        write_int16_le_file(fp,data->n);
        write_int16_le_file(fp,data->s);
        write_int16_le_file(fp,data->w);
        write_int16_le_file(fp,data->e);
        write_int16_le_file(fp,data->t);
        write_int16_le_file(fp,data->b);
        write_int16_le_file(fp,data->u[0]);
        write_int16_le_file(fp,data->u[1]);
        write_int16_le_file(fp,data->u[2]);
    }
    return ERR_NONE;
}

short switch_cubedata_entries(struct CUBES_DATA *cubes,unsigned long idx1,unsigned long idx2)
{
    if ((idx1>=cubes->count)||(idx2>=cubes->count))
        return XCUTX_INTERNAL;
    struct CUBE_TEXTURES data;
    memcpy(&data,&(cubes->data[idx1]),sizeof(struct CUBE_TEXTURES));
    memcpy(&(cubes->data[idx1]),&(cubes->data[idx2]),sizeof(struct CUBE_TEXTURES));
    memcpy(&(cubes->data[idx2]),&data,sizeof(struct CUBE_TEXTURES));
    return ERR_NONE;
}

short load_textureanim(struct CUBES_DATA *cubes,const char *fname)
{
    //Reading file
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    // Checking file size
    cubes->anitxcount=(mem->len>>4);
    if ((mem->len!=(cubes->anitxcount<<4)))
    {
      memfile_free(&mem);
      return XCUTX_FILE_BADDATA;
    }
    //Loading the entries
    cubes->anitx=malloc(cubes->anitxcount*sizeof(struct CUBE_TXTRANIM));
    int i,k;
    for (k=0; k<cubes->anitxcount; k++)
    {
      unsigned char *addr=mem->content + (k<<4);
      for (i=0; i<8; i++)
      {
        unsigned int val;
        val=read_int16_le_buf(addr+(i<<1));
        cubes->anitx[k].data[i]=val;
      }
    }
    memfile_free(&mem);
    return ERR_NONE;
}

short load_texture(unsigned char **texture,const char *fname)
{
    unsigned long texture_file_len = (TEXTURE_SIZE_X*TEXTURE_COUNT_X) * (TEXTURE_SIZE_Y*TEXTURE_COUNT_Y);
    //Reading file
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    // Checking file size
    if ((mem->len!=texture_file_len))
    {
      memfile_free(&mem);
      return XCUTX_FILE_BADDATA;
    }
    // Allocating buffer
    (*texture)=malloc(texture_file_len);
    if ((*texture)==NULL)
    {
      memfile_free(&mem);
      return XCUTX_MALLOC_ERR;
    }
    //Loading the entries
    int i;
    for (i=0; i<(TEXTURE_SIZE_Y*TEXTURE_COUNT_Y); i++)
    {
        unsigned long pos = (TEXTURE_SIZE_X*TEXTURE_COUNT_X) * i;
        memcpy((*texture)+pos,mem->content+pos,(TEXTURE_SIZE_X*TEXTURE_COUNT_X));
    }
    memfile_free(&mem);
    return ERR_NONE;
}

