/*
 * main.h   defines exported routines from main.c
 */

#ifndef ADIKT_MAIN_H
#define ADIKT_MAIN_H

#if defined(unix) && !defined(GO32)
#define SEPARATOR "/"
#else
#define SEPARATOR "\\"
#endif

static void read_init(void);
static void init_keys(void);
static void init_help(void);
static char ***match_title(char *title, int n);
static void init_slbkey(void);
static void strip_crlf(char *string_in);

#endif // ADIKT_MAIN_H
