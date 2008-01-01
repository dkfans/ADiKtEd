/*
 * scr_list.c
 *
 * Defines functions for initializing and displaying any
 * of the list screens.
 * This also includes keyboard actions for the screen.
 *
 */

#include "scr_list.h"

#include "globals.h"
#include "output_scr.h"
#include "input_kb.h"
#include "lev_data.h"
#include "scr_actn.h"
#include "scr_help.h"
#include "scr_thing.h"
#include "obj_things.h"
#include "obj_column.h"
#include "obj_slabs.h"
#include "lev_things.h"
#include "lev_column.h"

// Variables

LIST_DATA *list;

/*
 * Initializes variables for the list screen.
 */
short init_list(void)
{
    //Creating and clearing list variable
    list=(LIST_DATA *)malloc(sizeof(LIST_DATA));
    if (list==NULL)
     die("init_list: Cannot allocate memory.");
    return true;
}

/*
 * Deallocates memory for the list screen.
 */
void free_list(void)
{
  free(list);
}

/*
 * Covers actions from the creature screen.
 */
void actions_crtre(int key)
{
    int sx, sy;
    sx = (mapmode->mapx+mapmode->screenx)*3+mapmode->subtl_x;
    sy = (mapmode->mapy+mapmode->screeny)*3+mapmode->subtl_y;
    unsigned char *thing;
    message_release();
    if (!actions_list(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          end_list();
          message_info("Adding creature cancelled");
          break;
        case KEY_ENTER:
          thing = create_creature(sx,sy,list->pos+1);
          set_thing_tilepos_h(thing,1);
          set_thing_owner(thing,get_tile_owner(lvl,mapmode->mapx+mapmode->screenx,mapmode->mapy+mapmode->screeny));
          thing_add(lvl,thing);
          // Show the new thing
          mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
          end_list();
          message_info("Creature added");
          break;
        default:
          message_info("Unrecognized creature key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from the item type screen.
 */
void actions_itemt(int key)
{
    unsigned char *thing;
    int sx=(mapmode->mapx+mapmode->screenx)*3+mapmode->subtl_x;
    int sy=(mapmode->mapy+mapmode->screeny)*3+mapmode->subtl_y;
    message_release();
    if (!actions_list(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          end_list();
          message_info("Adding thing cancelled");
          break;
        case KEY_ENTER:
          tng_makeitem(sx,sy,list->pos+1);
          end_list();
          message_info("Item added");
          break;
        default:
          message_info("Unrecognized item key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from texture selection screen.
 */
void actions_mdtextr(int key)
{
    message_release();
    if (!actions_list(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          end_list();
          message_info("Texture change cancelled");
          break;
        case KEY_ENTER:
          lvl->inf=list->pos;
          end_list();
          message_info("Texture changed");
          break;
        default:
          message_info("Unrecognized texture key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from custom columns screen.
 */
void actions_mdcclm(int key)
{
    struct DK_CUSTOM_CLM *cclm_recs[9];
    struct COLUMN_REC *clm_recs[9];
    int tx=(mapmode->mapx+mapmode->screenx);
    int ty=(mapmode->mapy+mapmode->screeny);
    message_release();
    if (!actions_list(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          end_list();
          message_info("Customization cancelled");
          break;
        case KEY_ENTER:
          {
            int i,k;
            for (k=0;k<3;k++)
              for (i=0;i<3;i++)
              {
                cclm_recs[k*3+i]=create_cust_col(tx*3+i,ty*3+k);
                clm_recs[k*3+i]=cclm_recs[k*3+i]->rec;
              }
            //Retrieving parameters from LEVEL structure - the slab and its surrounding
            unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
            unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
            unsigned char **surr_tng=(unsigned char **)malloc(9*sizeof(unsigned char *));
            get_slab_surround(surr_slb,surr_own,surr_tng,tx,ty);
            fill_custom_column_data(list->pos,clm_recs,surr_slb,surr_own,surr_tng);
            for (k=0;k<3;k++)
              for (i=0;i<3;i++)
              {
                cust_col_add_or_update(lvl,&cclm_recs[k*3+i]);
              }
            end_list();
            message_info("Custom columns set");
          };break;
        default:
          message_info("Unrecognized custom columns key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from custom cubes screen.
 */
void actions_mdcube(int key)
{
    message_release();
    if (!actions_list(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          end_list();
          message_info("Customization cancelled");
          break;
        case KEY_ENTER:
          //TODO
          end_list();
          message_info("Column cubes set");
          break;
        default:
          message_info("Unrecognized custom cubes key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from slab list screen.
 */
void actions_mdslbl(int key)
{
    message_release();
    if (!actions_list(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          end_list();
          message_info("Slab change cancelled");
          break;
        case KEY_ENTER:
          slb_place_room(lvl,mapmode,list->pos);
          end_list();
          message_info("New slab placed");
          break;
        default:
          message_info("Unrecognized slab list key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from all screens with numbered list.
 */
short actions_list(int key)
{
    int num_rows=(list->items)/(list->cols)
                        + (((list->items)%(list->cols))>0);
    switch (key)
    {
    case KEY_UP:
      list->pos--;
      break;
    case KEY_DOWN:
      list->pos++;
      break;
    case KEY_LEFT:
      list->pos-=(list->items)/(list->cols)
                        + (((list->items)%(list->cols))>0);
      break;
    case KEY_RIGHT:
      list->pos+=(list->items)/(list->cols)
                        + (((list->items)%(list->cols))>0);
      break;
    case KEY_PGUP:
      list->pos-=scrmode->rows-1;
      break;
    case KEY_PGDOWN:
      list->pos+=scrmode->rows-1;
      break;
    default:
      return false;
    }
    if (list->pos >= list->items)
      list->pos=list->items-1;
    if (list->pos<0) list->pos=0;
    //Compute the row where selected item is
    int curr_row=(list->pos)%num_rows;
    if ((list->y)>curr_row) list->y=curr_row;
    if (list->y+scrmode->rows<curr_row+1) list->y=curr_row-scrmode->rows+1;
    if (list->y+scrmode->rows > num_rows+1)
      list->y=num_rows-scrmode->rows+1;
    if (list->y<0) list->y=0;
    return true;
}


/*
 * Action function - start any of the the list modes.
 */
short start_list(struct LEVEL *lvl,int lstmode)
{
    list->prevmode=scrmode->mode;
    list->pos=0; // selected item position (rel. to screen top)
    //These parameters will be reset to proper values on redraw
    list->y=0; //the first visible row
    list->items=8; //number of items in the list
    list->cols=2; //number of columns in the item list
    scrmode->mode=lstmode;
    message_info("Use arrow keys and page up/down to move, "
      "enter to choose.");
    return true;
}

/*
 * Action function - start texture selection mode.
 */
short start_mdtextr(struct LEVEL *lvl)
{
    short result;
    result=start_list(lvl,MD_TXTR);
    list->pos=lvl->inf;
    return result;
}

/*
 * Action function - start slab list mode.
 */
short start_mdslbl(struct LEVEL *lvl)
{
    int tx=mapmode->mapx+mapmode->screenx;
    int ty=mapmode->mapy+mapmode->screeny;
    short result;
    result=start_list(lvl,MD_SLBL);
    list->pos=get_tile_slab(lvl,tx,ty);
    return result;
}

/*
 * Action function - end any of the the list modes.
 */
void end_list()
{
    if (scrmode->mode!=list->prevmode)
      scrmode->mode=list->prevmode;
    else
      scrmode->mode=MD_SLB;
    message_release();
}

/*
 * Draws screen with a numbered list (as creature types or item types)
 * and key help for given screen.
 */
void draw_numbered_list(char *(*itemstr)(unsigned short),
        unsigned int start_idx,unsigned int end_idx,unsigned int itm_width)
{
    unsigned int line_length=min(scrmode->cols,LINEMSG_SIZE);
    unsigned int entry_width=itm_width+5;
    list->cols=max((unsigned int)(line_length/entry_width),1);
    list->items=end_idx-start_idx+1;
    unsigned int num_rows=list->items/list->cols
                        + (((list->items)%(list->cols))>0);
    unsigned int i,k;
    int col;
    for (i=0; i < scrmode->rows; i++)
    {
      set_cursor_pos(i, 0);
      char it_msg[LINEMSG_SIZE];
      if (i+list->y<num_rows)
        for (k=0;k<list->cols;k++)
        {
          int itm_idx=start_idx+list->y+i+k*num_rows;
          if (itm_idx-start_idx==list->pos)
              col=PRINT_COLOR_BLACK_ON_LGREY;
          else
              col=PRINT_COLOR_LGREY_ON_BLACK;
          if (itm_idx<=end_idx)
            sprintf(it_msg,"%3d: %-*.*s",
                    itm_idx,itm_width,itm_width, itemstr(itm_idx));
          else
            sprintf(it_msg,"");
          screen_setcolor(col);
          screen_printf("%s",it_msg);
        }
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf_toeol("");
      set_cursor_pos(i, scrmode->cols);
      screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
      screen_printchr(' ');
      screen_printchr(' ');
    }
    int scr_row=display_mode_keyhelp(0,scrmode->cols+3,scrmode->mode);
}

void draw_crtre()
{
    int all_rows=get_screen_rows();
    int all_cols=get_screen_cols();

    draw_numbered_list(get_creature_subtype_fullname,1,CREATR_SUBTP_FLOAT,16);
    int scr_col1=scrmode->cols+3;
    // Display more info about the creature
    if (all_rows > 7)
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(all_rows-10, scr_col1);
      screen_printf("%s","Creature parameters");
      set_cursor_pos(all_rows-8, scr_col1);
      screen_printf("Level: %d",1);
      set_cursor_pos(all_rows-6, scr_col1);
      screen_printf("Owner: %s",get_owner_type_fullname(1));
    }
    set_cursor_pos(get_screen_rows()-1, 17);
}

void draw_itemt()
{
    draw_numbered_list(get_item_subtype_fullname,1,ITEM_SUBTYPE_SPELLARMG,18);
    set_cursor_pos(get_screen_rows()-1, 17);
}

void draw_mdtextr()
{
    draw_numbered_list(get_texture_fullname,0,INF_MAX_INDEX,55);
    set_cursor_pos(get_screen_rows()-1, 17);
}

void draw_mdcclm()
{
    draw_numbered_list(get_custom_column_fullname,0,CUST_CLM_GEN_MAX_INDEX,18);
    set_cursor_pos(get_screen_rows()-1, 17);
}

void draw_mdcube()
{
    draw_mdempty();
//    draw_numbered_list(get_item_subtype_fullname,1,ITEM_SUBTYPE_SPELLARMG,18);
    set_cursor_pos(get_screen_rows()-1, 17);
}

void draw_mdslbl()
{
    draw_numbered_list(get_slab_fullname,0,SLAB_TYPE_GUARDPOST,17);
    set_cursor_pos(get_screen_rows()-1, 17);
}
