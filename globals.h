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

// Buffer sizes

#define DISKPATH_SIZE   256
#define LINEMSG_SIZE    80
#define READ_BUFSIZE    256

// File handling variables
extern char *filebase;

#endif // ADIKT_GLOBALS_H
