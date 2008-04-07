/*
 * scr_txted.h header file for scr_txted.c
 */

#ifndef ADIKT_SCRTXTED_H
#define ADIKT_SCRTXTED_H

// Variables

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;

struct TXTED_DATA {
    int prevmode;
    struct DK_SCRIPT *script;
    int y;
    int top_row;
    int err_row;
    int err_param;
  };

struct TXTED_COLORS {
    int command;
    int spaces;
    int comment;
    int bad;
    int param_text;
    int param_num;
//    int param_player;
  };

//Functions - init and free
short init_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - start and stop
short start_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - actions and screen
void actions_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - lower level
void script_verify_with_highlight(const struct LEVEL *lvl,struct TXTED_DATA *editor);

//Functions - internal

#endif // ADIKT_SCRTXTED_H
