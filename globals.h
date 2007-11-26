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
# include <unistd.h>
# include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#elseif defined(MSDOS)
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

// File handling variables
extern char *filebase;

// 0 for high-low, 1 for low-high
extern int datmode;

#endif // ADIKT_GLOBALS_H
