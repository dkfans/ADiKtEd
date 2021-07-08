/******************************************************************************/
/** @file bulcommn.c
 * Various functions used in many tools for Bullfrog games.
 * @par Purpose:
 *     Common procedures for Bullfrog game tools. These includes
 *     bitmaps r/w functions compression detection and similar.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     10 Mar 2005 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lbfileio.h"

#include "bulcommn.h"

/**
 * RNC compression magic identifier, as string.
 */
char const *RNC_SIGNATURE_STR="RNC\001";

/**
 * Writes indexed colour bitmap into file with given name.
 * @param fname Destination file name.
 * @param width Bitmap width.
 * @param height Bitmap height; may be negative to flip the bitmap.
 * @param pal RGB Palette buffer.
 * @param data Bitmap data buffer.
 * @param red Red color position in patette.
 * @param green Green color position in patette.
 * @param blue Blue color position in patette.
 * @param mult Color value multiplier.
 * @return Returns 0 on success, error code on failure.
 */
short write_bmp_fn_idx (const char *fname, int width, int height, 
        const unsigned char *pal, const char *data,
        int red, int green, int blue, int mult)
{
  FILE *out;
  out = fopen (fname, "wb");
  if (out==NULL)
  {
      printf ("\nCan't open file %s.\n", fname);
      return 1;
  }

  short result;
  result=write_bmp_fp_idx(out,width,height,pal,data,red,green,blue,mult);
  fclose (out);
  return result;
}

/**
 * Writes indexed colour bitmap into opened file.
 * @param out Destination file (already opened for writing).
 * @param width Bitmap width.
 * @param height Bitmap height; may be negative to flip the bitmap.
 * @param pal RGB Palette buffer.
 * @param data Bitmap data buffer.
 * @param red Red color position in patette.
 * @param green Green color position in patette.
 * @param blue Blue color position in patette.
 * @param mult Color value multiplier.
 * @return Returns 0 on success, error code on failure.
 */
short write_bmp_fp_idx (FILE *out, int width, int height, 
        const unsigned char *pal, const char *data,
        int red, int green, int blue, int mult)
{
  int pwidth, pheight;
  long data_len,pal_len;
  int i, j;
    
  /* Positive width and height */
  if (width>=0)
    pwidth=width;
  else
    pwidth=-width;
  if (height>=0)
    pheight=height;
  else
    pheight=-height;
  int padding_size=4-(pwidth&3);
  /* Length of data */
  data_len = (pwidth+padding_size)*pheight;
  /* Length of palette */
  pal_len = 256*4;
  fputs("BM",out);
  write_int32_le_file(out, data_len+pal_len+0x36);
  write_int32_le_file(out, 0);
  write_int32_le_file(out, pal_len+0x36);
  write_int32_le_file(out, 40);
  write_int32_le_file(out, width);
  write_int32_le_file(out, height);
  write_int16_le_file(out, 1);
  write_int16_le_file(out, 8);
  write_int32_le_file(out, 0);
  write_int32_le_file(out, 0);
  write_int32_le_file(out, 0);
  write_int32_le_file(out, 0);
  write_int32_le_file(out, 0);
  write_int32_le_file(out, 0);
    
  for (i=0; i < 256; i++)
  {
      unsigned int cval;
      cval=(unsigned int)pal[i*3+blue]*mult;
      if (cval>255) cval=255;
      fputc(cval, out);
      cval=(unsigned int)pal[i*3+green]*mult;
      if (cval>255) cval=255;
      fputc(cval, out);
      cval=(unsigned int)pal[i*3+red]*mult;
      if (cval>255) cval=255;
      fputc(cval, out);
      fputc(0, out);
  }
    
  for (i=1; i <= pheight; i++)
  {
    fwrite (data+(pheight-i)*pwidth, pwidth, 1, out);
    if ((padding_size&3) > 0)
      for (j=0; j < padding_size; j++)
        fputc (0, out);
  }
    
  return 0;
}

/**
 * Writes indexed colour bitmap into file with given name.
 * @param fname Destination file name.
 * @param width Bitmap width.
 * @param height Bitmap height.
 * @param data Bitmap data buffer.
 * @return Returns 0 on success, error code on failure.
 */
short write_bmp_fn_24b (const char *fname, int width, int height, const char *data)
{
  FILE *out;
  out = fopen (fname, "wb");
  if (out==NULL)
  {
      printf ("\nCan't open file %s.\n", fname);
      return 1;
  }

  short result;
  result=write_bmp_fp_24b(out,width,height,data);
  fclose (out);
  return result;
}

/**
 * Writes indexed colour bitmap into opened file.
 * @param out Destination file (already opened for writing).
 * @param width Bitmap width.
 * @param height Bitmap height.
 * @param data Bitmap data buffer.
 * @return Returns 0 on success, error code on failure.
 */
short write_bmp_fp_24b(FILE *out, int width, int height, const char *data)
{
  int pwidth, pheight;
  long data_len;
  int i;
    
  /* Positive width and height */
  if (width>=0)
    pwidth=width;
  else
    pwidth=-width;
  if (height>=0)
    pheight=height;
  else
    pheight=-height;
    
  int datawidth=width*3;
  int padding_size=4-(datawidth&3);
  /* Length of data */
  data_len = (pwidth+padding_size)*pheight;
  fputs("BM",out);
  write_int32_le_file (out, 3*data_len+0x36);
  write_int32_le_file (out, 0);
  write_int32_le_file (out, 0x36);
  write_int32_le_file (out, 40);
  write_int32_le_file (out, width);
  write_int32_le_file (out, height);
  write_int16_le_file (out, 1);
  write_int16_le_file (out, 24);
  write_int32_le_file (out, 0);
  write_int32_le_file (out, 0);
  write_int32_le_file (out, 0);
  write_int32_le_file (out, 0);
  write_int32_le_file (out, 0);
  write_int32_le_file (out, 0);
    
    for (i=1; i <= pheight; i++)
    {
        fwrite (data+(pheight-i)*datawidth, datawidth, 1, out);
        if ((padding_size&3) > 0)
        {
            int cntr;
            for (cntr=0;cntr<padding_size;cntr++)
                fputc (0, out);
        }
    }
    
    fclose (out);
    return 0;
}


/**
 * Reads RGB palette file into preallocated buffer.
 * The input file can't be compressed and must have 768 bytes.
 * @return Returns 0 on success, error code on failure.
 */
int read_palette_rgb(unsigned char *palette, const char *fname, unsigned int nColors)
{
    FILE *palfp;
    palfp = fopen (fname, "rb");
    if (!palfp) return 1;
    int palSize=3*nColors;
    int readed=fread (palette, 1, palSize, palfp);
    fclose(palfp);
    if (palSize!=readed) return 2;
    return 0;
}

/**
 * Returns a random number within given range.
 */
unsigned int rnd(const unsigned int range)
{
    return (rand()%(range));
}

/**
 * Gives version of the RNC file.
 * Requies buff to be at least 4 bytes long.
 * @param buff The source buffer.
 * @return Returns 0 if buff seems not to be RNC compressed,
 *     or RNC version (positive number) if buff is compressed.
 */
int rnc_compressed_buf (char *buff)
{
    if (strncmp(buff,RNC_SIGNATURE_STR,3)!=0)
        return 0;
    int rncver=buff[3];
    if (rncver==0) rncver=255;
    return rncver;
}

/**
 * Gives version of the RNC file.
 * Reads 4 starting bytes of given FILE, and compares three of them with
 * RNC signature. Then returns fourth one.
 * @param fp The opened file.
 * @return Returns 0 if file seems not to be RNC compressed,
 *     or RNC version (positive number) if file is compressed.
 */
int rnc_compressed_file (FILE *fp)
{
    char buff[5];
    long lastpos = ftell (fp);
    fseek (fp, 0, SEEK_SET);
    int readed=fread (buff, 1, 4, fp);
    fseek (fp, lastpos, SEEK_SET);
    if (readed<4) return 0;
    return rnc_compressed_buf(buff);
}
