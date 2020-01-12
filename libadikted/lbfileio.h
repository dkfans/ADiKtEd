/******************************************************************************/
/** @file lbfileio.h
 * Library for r/w of little/big endian binary data from files.
 * @par Purpose:
 *     Header file. Defines exported routines from lbfileio.c
 * @par Comment:
 *   Those are simple, low-level functions, so all are defined as inline.
 * @author   Tomasz Lis
 * @date     11 Mar 2005 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef LBFILEIO_H
#define LBFILEIO_H

# include <stdio.h>

// Routines

long file_length (char *path);
long file_length_opened (FILE *fp);

long read_int32_le_file (FILE *fp);
long read_int32_le_buf (const unsigned char *buff);
unsigned short read_int16_le_file (FILE *fp);
unsigned short read_int16_le_buf (const unsigned char *buff);

void write_int16_le_file (FILE *fp, unsigned short x);
void write_int16_le_buf (unsigned char *buff, unsigned short x);
void write_int32_le_file (FILE *fp, unsigned long x);
void write_int32_le_buf (unsigned char *buff, unsigned long x);

long read_int32_be_file (FILE *fp);
long read_int32_be_buf (const unsigned char *buff);
unsigned short read_int16_be_file (FILE *fp);
unsigned short read_int16_be_buf (const unsigned char *buff);

void write_int16_be_file (FILE *fp, unsigned short x);
void write_int16_be_buf (unsigned char *buff, unsigned short x);
void write_int32_be_file (FILE *fp, unsigned long x);
void write_int32_be_buf (unsigned char *buff, unsigned long x);

unsigned char read_int8_buf (const unsigned char *buff);
short nth_bit( unsigned char c, short n );
short nth_bit_fourbytes( unsigned char c[4], short n );

#endif
