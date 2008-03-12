/*
 * scr_help.h header file for scr_help.c
 */

#ifndef ADIKT_SCRHELP_H
#define ADIKT_SCRHELP_H

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;

// Help variables
typedef struct {
    int formode;
    int y;
    int rows;
    char **text;

    int clmrows;
    char **clm;
    int slbkeyrows;
    char **slbkey;
    int tngkeyrows;
    char **tngkey;
    int crtkeyrows;
    char **crtkey;
    int itmtkeyrows;
    char **itmtkey;
    int txtrkeyrows;
    char **txtrkey;
    int cclmkeyrows;
    char **cclmkey;
    int cubekeyrows;
    char **cubekey;
    int slblkeyrows;
    char **slblkey;
    int srchkeyrows;
    char **srchkey;
    int slbrows;
    char **slb;
    int tngrows;
    char **tng;
    int crtrows;
    char **crt;
    int itmtrows;
    char **itmt;
    int scrprows;
    char **scrp;
    int txtrrows;
    char **txtr;
    int cclmrows;
    char **cclm;
    int cuberows;
    char **cube;
    int slblrows;
    char **slbl;
    int srchrows;
    char **srch;
    int rwrkrows;
    char **rwrk;
    int tiprows;
    char **tips;
    int compassrows;
    char **compass;
  } HELP_DATA;

extern HELP_DATA *help;

//Functions - init and free
short init_help(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
void free_help(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
short init_key_help(int mode);

//Functions - start and stop
short start_help(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void end_help(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - actions and screen
void actions_help(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_help(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - lower level
void draw_help_line(int posy,int posx,char *text);
char *get_random_tip();

//Functions - internal
char ***match_title(char *title, int n);

#endif // ADIKT_SCRHELP_H
