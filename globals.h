/*
 * globals.h header file
 *
 * Defines basic includes and definitions, used in whole program
 *
 */

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

// Return values for verification functions
#define VERIF_ERROR   0
#define VERIF_OK      1
#define VERIF_WARN    2

#if defined(unix) && !defined(GO32)
// Externs for updating console on resize
extern volatile int safe_update, update_required;
#endif

struct memory_file
{
    long len;
    unsigned char *content;
};

struct memory_file read_file (char *iname);

// File handling variables
extern char *filebase;

// The way DAT entries are shown
extern short dat_view_mode;
extern short show_obj_range;

// True means DAT/CLM/WIB are updated automatically
extern short datclm_auto_update;
// True means TNG/LGT/APTs are updated automatically
extern short obj_auto_update;

#endif // ADIKT_GLOBALS_H
