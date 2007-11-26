/*
 * scr_help.h header file for scr_help.c
 */

#ifndef ADIKT_SCRHELP_H
#define ADIKT_SCRHELP_H

extern void init_help(void);
extern char ***match_title(char *title, int n);
extern void draw_help(void);

// Help variables

extern int helpformode;
extern int helpy;

extern int clmhelprows;
extern char **clmhelp;
extern int slbkeyhelprows;
extern char **slbkeyhelp;
extern int crtkeyhelprows;
extern char **crtkeyhelp;
extern int itmtkeyhelprows;
extern char **itmtkeyhelp;
extern int slbhelprows;
extern char **slbhelp;
extern int tnghelprows;
extern char **tnghelp;
extern int crthelprows;
extern char **crthelp;
extern int itmthelprows;
extern char **itmthelp;
extern int helprows;
extern char **helptext;

#endif // ADIKT_SCRHELP_H
