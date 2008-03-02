/*
 * graffiti.h header file for graffiti.c
 */

#ifndef ADIKT_GRAFFITI_H
#define ADIKT_GRAFFITI_H

struct LEVEL;
struct COLUMN_REC;
struct DK_GRAFFITI;

// Imports from graffiti.c
short level_free_graffiti(struct LEVEL *lvl);
int place_graffiti_on_slab(struct COLUMN_REC *clm_recs[9],struct LEVEL *lvl, int tx, int ty);

struct DK_GRAFFITI *get_graffiti(struct LEVEL *lvl, int graf_idx);
char *get_graffiti_text(struct LEVEL *lvl,unsigned int num);
int graffiti_idx(struct LEVEL *lvl, int tx, int ty);
int graffiti_idx_next(struct LEVEL *lvl, int tx, int ty,int prev_idx);
struct DK_GRAFFITI *create_graffiti(int tx, int ty, char *text, int orient);
int graffiti_add_obj(struct LEVEL *lvl,struct DK_GRAFFITI *graf);
int graffiti_add(struct LEVEL *lvl,int tx, int ty,int height, char *text,int font,
      unsigned short orient,unsigned short cube);
void graffiti_del(struct LEVEL *lvl,unsigned int num);
short place_graffiti_on_column(struct COLUMN_REC *clm_rec,unsigned short font,
        unsigned short height,char *text,int graf_subtl,unsigned short cube);
short place_graffiti_on_clm_top(struct COLUMN_REC *clm_rec,unsigned short font,
        unsigned short height,char *text,int graf_subtl,int graf_subtl_h,
        unsigned short cube);
int compute_graffiti_subtl_length(unsigned short font,char *text);
void graffiti_update_columns(struct LEVEL *lvl,int graf_idx);
void graffiti_clear_from_columns(struct LEVEL *lvl,int graf_idx);
short set_graffiti_orientation(struct DK_GRAFFITI *graf,unsigned short orient);
int set_graffiti_height(struct DK_GRAFFITI *graf,int height);

int get_graffiti_cube_height(unsigned short font,char *text);
const unsigned char *get_font_char(unsigned short font,char chr);


// Import the font from graffiti_font.c
extern const unsigned char font_any_chnull[];
extern const unsigned char *font_adiclssc_chars[];
extern const int font_adiclssc_height;
extern const unsigned char *font_adisize8_chars[];
extern const int font_adisize8_height;

#endif // ADIKT_GRAFFITI_H
