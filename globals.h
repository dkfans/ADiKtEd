/******************************************************************************/
// dernc.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  08 sep 1998
// Modified: Tomasz Lis

// Purpose:
//   Header file for global definitions.

// Comment:
//   Defines basic includes and definitions, used in whole program.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_GLOBALS_H
#define ADIKT_GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#if defined(unix) && !defined(GO32)
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(MSDOS)
#include <dos.h>
#include <process.h>
#endif

// Basic Definitions

#if defined(unix) && !defined (GO32)
#define SEPARATOR "/"
#else
#define SEPARATOR "\\"
#endif

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

// Buffer sizes

#define DISKPATH_SIZE   256
#define LINEMSG_SIZE    160
#define READ_BUFSIZE    256

#define PROGRAM_NAME "ADiKtEd"

// Return values for verification functions
#define VERIF_ERROR   0
#define VERIF_OK      1
#define VERIF_WARN    2

#if defined(unix) && !defined(GO32)
// Externs for updating console on resize
extern volatile int safe_update, update_required;
#endif

struct IPOINT_2D {
    int x;
    int y;
};

struct IPOINT_3D {
    int x;
    int y;
    int z;
};

struct UPOINT_2D {
    unsigned int x;
    unsigned int y;
};

struct UPOINT_3D {
    unsigned int x;
    unsigned int y;
    unsigned int z;
};

struct USPOINT_2D {
    unsigned short x;
    unsigned short y;
};

struct IRECT_2D {
    int l;
    int r;
    int t;
    int b;
};

struct LEVOPTIONS {
    // Level graphic generation options
    short unaffected_gems;
    short unaffected_rock;
    short fill_reinforced_corner;
    short frail_columns;
  };

// File handling variables
extern char *levels_path;
extern char *data_path;

// Thing categorization helpers
#define THING_CATEGR_COUNT 0x16
typedef short (*is_thing_subtype)(const unsigned char *thing);
typedef short (*is_item_subtype)(const unsigned char stype_idx);
typedef int (*thing_subtype_arrayindex)(const unsigned char stype_idx);
typedef unsigned char (*thing_subtype_switch)(const unsigned char stype_idx);
typedef unsigned char (*thing_subtype_arrayitem)(const int arr_itm);

// True means DAT/CLM/WIB are updated automatically
extern short datclm_auto_update;
// True means TNG/LGT/APTs are updated automatically
extern short obj_auto_update;

#endif // ADIKT_GLOBALS_H
