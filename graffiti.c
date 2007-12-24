/*
 * graffiti.c
 *
 * Module for handling wall graffities.
 *
 */

#include "graffiti.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_things.h"
#include "obj_slabs.h"
#include "obj_column.h"

/*
 * Frees the whole graffiti structure
 */
void free_graffiti(struct LEVEL *lvl)
{
    int i;
    for (i=lvl->graffiti_count-1; i>=0 ; i--)
    {
      free (lvl->graffiti[i]->text);
      free (lvl->graffiti[i]);
    }
    if (lvl->graffiti_count>0)
      free(lvl->graffiti);
}

/*
 * Returns index of first graffiti at given tile, or -1 if not found.
 */
int graffiti_idx(struct LEVEL *lvl, int tx, int ty)
{
    return graffiti_idx_next(lvl, tx, ty,-1);
}

/*
 * Returns index of next graffiti at given tile,
 * the one after prev_idx; or -1 if not found.
 * To get first graffiti, prev_idx must be -1.
 */
int graffiti_idx_next(struct LEVEL *lvl, int tx, int ty,int prev_idx)
{
    if (prev_idx<-1) return -1;
    int i;
    struct DK_GRAFFITI *graf;
    for (i=prev_idx+1; i < lvl->graffiti_count; i++)
    {
      graf = lvl->graffiti[i];
      if ((tx>=graf->tx) && (tx<=graf->fin_tx) && (ty>=graf->ty) && (ty<=graf->fin_ty))
          return i;
      // This makes empty/wrong graffitis visible
      if ((ty==graf->ty) && (tx==graf->tx))
          return i;
    }
    return -1;
}

char *get_graffiti_text(struct LEVEL *lvl,unsigned int num)
{
    if ((num>=lvl->graffiti_count) || (num<0))
      return "";
    return lvl->graffiti[num]->text;
}

void graffiti_del(struct LEVEL *lvl,unsigned int num)
{
    int i;
    if ((lvl==NULL)||(num>=lvl->graffiti_count))
      return;
    struct DK_GRAFFITI *graf;
    graf=lvl->graffiti[num];
    if (graf!=NULL)
    {
      free(graf->text);
      free(graf);
    }
    int graff_max_idx=lvl->graffiti_count-1;
    for (i=num; i < graff_max_idx; i++)
    {
      lvl->graffiti[i]=lvl->graffiti[i+1];
    }
    //Decrease the graffiti_count by one
    lvl->graffiti_count=graff_max_idx;
    //Decrease amount of allocated memory, or free the block
    lvl->graffiti = (struct DK_GRAFFITI **)realloc(lvl->graffiti,
             (graff_max_idx)*sizeof(struct DK_GRAFFITI *));
}

struct DK_GRAFFITI *create_graffiti(int tx, int ty, char *text, int orient)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    tx%=MAP_SIZE_X;
    ty%=MAP_SIZE_Y;

    if (text==NULL) return NULL;

    int i;
    int l;
    l = strlen(text);
    //Computing graffiti length in subtiles
    int subtl_len=0;
    for (i=0; i<l; i++)
    {
      //The zero index in every chars[] entry is number of column
      //needed for this character. The "+1" is required
      // to make space between letters
      subtl_len+=chars[(unsigned char)text[i]][0]+1;
      int graf_end_subtl_x,graf_end_subtl_y;
      switch (orient)
      {
      case ORIENT_NS:
      case ORIENT_SN:
          graf_end_subtl_x=tx*3;
          graf_end_subtl_y=ty*3+subtl_len;
          break;
      case ORIENT_WE:
      case ORIENT_EW:
          graf_end_subtl_x=tx*3+subtl_len;
          graf_end_subtl_y=ty*3;
          break;
      default:
          graf_end_subtl_x=tx*3;
          graf_end_subtl_y=ty*3;
          break;
      }
      //If we've exceeded map space - truncate the graffiti
      if ((graf_end_subtl_x > arr_entries_x)||(graf_end_subtl_y > arr_entries_y))
      {
        l=i;
        break;
      }
    }
    // Remove space after last character
    subtl_len--;
    // If there's not a single char to process (string empty at start,
    // or truncated before) - exit
    if (l<1) return NULL;
    //We will need string length in tiles
    int tiles_len=(subtl_len/3) + ((subtl_len%3)>0);
    if (tiles_len<1) return NULL;
    struct DK_GRAFFITI *graf;
    //Filling graffiti structure
    graf = (struct DK_GRAFFITI *)malloc(sizeof(struct DK_GRAFFITI));
    if (graf==NULL) die("Cannot alloc memory for graffiti item");
    graf->tx=tx;
    graf->ty=ty;
    graf->orient=orient;
    graf->font=GRAFF_FONT_ADICLSSC;
    graf->height=0;
    graf->cube=0x0184;
    graf->text = (char *)malloc((l+1)*sizeof(char));
    strncpy(graf->text,text,l);
    graf->text[l]='\0';
    switch (orient)
    {
      case ORIENT_NS:
      case ORIENT_SN:
           graf->fin_tx = tx;
           graf->fin_ty = ty+tiles_len-1;
           break;
      case ORIENT_WE:
      case ORIENT_EW:
           graf->fin_tx = tx+tiles_len-1;
           graf->fin_ty = ty;
           break;
      default:
           graf->fin_tx = tx+tiles_len-1;
           graf->fin_ty = ty+tiles_len-1;
           break;
    }
    return graf;
}

/*
 * Adds graffiti object to level data, without filling the graffiti
 * nor updating slabs.
 */
int graffiti_add_obj(struct LEVEL *lvl,struct DK_GRAFFITI *graf)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    if ((lvl==NULL)||(graf==NULL)) return -1;

    int graf_idx=lvl->graffiti_count;
    lvl->graffiti = (struct DK_GRAFFITI **)realloc (lvl->graffiti,
                     (graf_idx+1)*sizeof(struct DK_GRAFFITI *));
    if (lvl->graffiti==NULL) die("Cannot alloc memory for graffiti array");
    lvl->graffiti[graf_idx]=graf;
    lvl->graffiti_count=graf_idx+1;
    return graf_idx;
}

/*
 * Creates and adds graffiti.
 */
int graffiti_add(struct LEVEL *lvl,int tx, int ty,int height, char *text,int font,
      unsigned short orient,unsigned short cube)
{
    if ((lvl==NULL)||(text==NULL)||(strlen(text)<1)) return -1;
    struct DK_GRAFFITI *graf=create_graffiti(tx,ty,text,orient);
    if (graf==NULL) return false;
    graf->font=font;
    graf->height=height;
    graf->cube=cube;
    int graf_idx=graffiti_add_obj(lvl,graf);
    if (graf_idx<0)
      free(graf);
    return graf_idx;
}

/*
* Updates CLM entries to make the graffiti visible.
*/
void graffiti_update_columns(struct LEVEL *lvl,int graf_idx)
{
    if (graf_idx<0) return;
    struct DK_GRAFFITI *graf;
    graf=get_graffiti(lvl, graf_idx);
    if (graf==NULL) return;
    update_datclm_for_square(lvl,graf->tx,graf->fin_tx,graf->ty,graf->fin_ty);
}

struct DK_GRAFFITI *get_graffiti(struct LEVEL *lvl, int graf_idx)
{
    if ((graf_idx<0)||(graf_idx>=lvl->graffiti_count)) return NULL;
    return lvl->graffiti[graf_idx];
}

/*
 * Computes graffiti text length in subtiles, for given font.
 */
int compute_graffiti_subtl_length(unsigned short font,char *text)
{
    if (text==NULL) return 0;
    int l = strlen(text);
    if (l<1) return 0;
    switch (font)
    {
    case GRAFF_FONT_ADICLSSC:
         break;
    case GRAFF_FONT_NONE:
    default:
        return 0;
    }
    //Counting graffiti length in subtiles
    int i;
    int subtl_len=0;
    for (i=0; i < l; i++)
    {
        //The zero index in every chars[] entry is number of column
        //needed for this character. The "+1" is required
        // to make space between letters
        subtl_len+=chars[(unsigned char)text[i]][0]+1;
    }
    // Remove space after last char
    subtl_len--;
    return subtl_len;
}

/*
 * Draws graffiti on given columns array. Returns num of changed entries.
 */
int draw_graffiti_on_slab(struct COLUMN_REC *clm_recs[9],struct LEVEL *lvl, int tx, int ty)
{
    static const int dy[4][3]={{0,1,2}, {2,2,2}, {2,1,0}, {0,0,0}};
    static const int dx[4][3]={{0,0,0}, {0,1,2}, {2,2,2}, {2,1,0}};
    int mod_clms=0;
    int graf_idx=-1;
    while ((graf_idx=graffiti_idx_next(lvl,tx,ty,graf_idx))>=0)
    {
      struct DK_GRAFFITI *graf;
      graf=get_graffiti(lvl,graf_idx);
      if (graf==NULL) continue;
      //Setting some local variables
      int i;
      int base_sx=graf->tx*3;
      int base_sy=graf->ty*3;
      //Counting graffiti length in subtiles
      int subtl_len=compute_graffiti_subtl_length(graf->font,graf->text);
      //Starting part of the graffiti
      int graf_subtl_start;
      // Get short access to orientation, and make sure it won't exceed
      // dx[][]/dy[][] array
      unsigned short or=((graf->orient%4));
      switch (or)
      {
      case ORIENT_NS: graf_subtl_start=ty*3-base_sy; break;
      case ORIENT_SN: graf_subtl_start=(base_sy+subtl_len)-ty*3; break;
      case ORIENT_WE: graf_subtl_start=tx*3-base_sx; break;
      case ORIENT_EW: graf_subtl_start=(base_sx+subtl_len)-tx*3; break;
      default: graf_subtl_start=0; break;
      }
      for (i=0; i<3; i++)
      {
          int subtl=dy[or][i]*3 + dx[or][i];
          int graf_subtl=graf_subtl_start+i;
          short modified=draw_graffiti_on_column(clm_recs[subtl],graf->font,
                graf->text,graf_subtl,graf->cube);
          if (modified) mod_clms++;
      }
    }
    return mod_clms;
}

short draw_graffiti_on_column(struct COLUMN_REC *clm_rec,unsigned short font,
        char *text,int graf_subtl,unsigned short cube)
{
    if ((clm_rec==NULL)||(text==NULL)||(strlen(text)<1)) return false;
    int i;
    int l = strlen(text);
    int text_pos=0; //position of the character to print inside text
    int clm_pos=0; //index of the current column in the current character
    i=0;//temporary column counter
    for (text_pos=0; text_pos<l; text_pos++)
    {
        //The zero index in every chars[] entry is number of column
        //needed for this character.
        int chr_clms_count=chars[(unsigned char)(text[text_pos])][0]+1;
        if ((i<=graf_subtl)&&(i+chr_clms_count>graf_subtl))
        {
            //clm_pos starts with 1, not with 0 (because chars[][0] is
            // the number of columns in letter)
            clm_pos=graf_subtl-i+1;
            break;
        }
        i+=chr_clms_count;
    }
    //Check if we've found the right position
    if (text_pos>=l) return false;
    unsigned char *char_data=chars[(unsigned char)text[text_pos]];
    // Check if this is empty column (space between letters)
    if ((clm_pos<=0)||(clm_pos>char_data[0])) return false;
    unsigned char clm_mask=char_data[clm_pos];
    for (i=0;i<8;i++)
    {
        if ((clm_mask>>i)&0x01)
            clm_rec->c[i]=cube;
    }
    clm_rec->solid=compute_clm_rec_solid(clm_rec);
    clm_rec->height=compute_clm_rec_height(clm_rec);
    return true;
}
