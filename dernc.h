/*
 * dernc.h   define exported routines from dernc.c
 */

#ifndef RNC_DERNC_H
#define RNC_DERNC_H

/*
 * Routines
 */
long rnc_ulen (void *packed);
#ifndef COMPRESSOR
long rnc_unpack (void *packed, void *unpacked, unsigned int flags);
#else
long rnc_unpack (void *packed, void *unpacked, unsigned int flags, long *leeway);
#endif
char *rnc_error (long errcode);
long rnc_crc (void *data, unsigned long len);

/*
 * Error returns
 */
#define RNC_FILE_IS_NOT_RNC    -1
#define RNC_HUF_DECODE_ERROR   -2
#define RNC_FILE_SIZE_MISMATCH -3
#define RNC_PACKED_CRC_ERROR   -4
#define RNC_UNPACKED_CRC_ERROR -5
#define RNC_HEADER_VAL_ERROR   -6
#define RNC_HUF_EXCEEDS_RANGE  -7

/*
 * Flags to ignore errors
 */
#define RNC_IGNORE_FILE_IS_NOT_RNC    0x0001
#define RNC_IGNORE_HUF_DECODE_ERROR   0x0002
#define RNC_IGNORE_FILE_SIZE_MISMATCH 0x0004
#define RNC_IGNORE_PACKED_CRC_ERROR   0x0008
#define RNC_IGNORE_UNPACKED_CRC_ERROR 0x0010
#define RNC_IGNORE_HEADER_VAL_ERROR   0x0020
#define RNC_IGNORE_HUF_EXCEEDS_RANGE  0x0040

/*
 * The compressor needs this define
 */
#ifdef INTERNAL
#define RNC_SIGNATURE 0x524E4301       /* "RNC\001" */
#endif

#endif
