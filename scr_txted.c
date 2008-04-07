/*
 * scr_txted.c
 *
 * Defines functions for initializing and displaying the scrpt screen.
 * This also includes keyboard actions for the screen.
 *
 */

#include "scr_txted.h"

#include "globals.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "lev_data.h"

// Constants

const struct TXTED_COLORS script_std_colors = {
    PRINT_COLOR_WHITE_ON_BLACK,
    PRINT_COLOR_LBLUE_ON_BLACK,
    PRINT_COLOR_LGREY_ON_BLACK,
    PRINT_COLOR_LRED_ON_BLACK,
    PRINT_COLOR_LCYAN_ON_BLACK,
    PRINT_COLOR_LGREEN_ON_BLACK,
  };

const struct TXTED_COLORS script_hl_colors = {
    PRINT_COLOR_WHITE_ON_GREY,
    PRINT_COLOR_BLUE_ON_LGREY,
    PRINT_COLOR_LGREY_ON_GREY,
    PRINT_COLOR_LRED_ON_YELLOW,//PRINT_COLOR_RED_ON_WHITE,
    PRINT_COLOR_CYAN_ON_WHITE,
    PRINT_COLOR_GREEN_ON_WHITE,
  };

// Variables

/*
 * Initializes variables for the scrpt screen.
 */
short init_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    //Creating and clearing scrpt variable
    workdata->editor=(struct TXTED_DATA *)malloc(sizeof(struct TXTED_DATA));
    if (workdata->editor==NULL)
     die("init_scrpt: Cannot allocate memory.");
    return true;
}

/*
 * Deallocates memory for the scrpt screen.
 */
void free_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
  free(workdata->editor);
}


/*
 * Covers actions from the scrpt screen.
 */
void actions_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    int rows;
    rows=workdata->editor->script->lines_count;
    switch (key)
    {
    case KEY_UP:
      workdata->editor->y--;
      break;
    case KEY_DOWN:
      workdata->editor->y++;
      break;
    case KEY_PGUP:
      workdata->editor->y-=scrmode->rows-1;
      break;
    case KEY_PGDOWN:
      workdata->editor->y+=scrmode->rows-1;
      break;
    case KEY_U:
      recompute_script_levels(&(workdata->lvl->script));
      recompose_script(&(workdata->lvl->script));
      break;
        case KEY_V: // Verify whole map
          script_verify_with_highlight(workdata->lvl,workdata->editor);
          break;
    case KEY_TAB:
    case KEY_ESCAPE:
      end_scrpt(scrmode,workdata);
      message_info("Returned to last work mode");
      break;
    }
    //Fixing y
    if (workdata->editor->y >= rows)
      workdata->editor->y=rows-1;
    if (workdata->editor->y < 0)
      workdata->editor->y=0;
    //Fixing top_row based on y
    if (workdata->editor->top_row+scrmode->rows < workdata->editor->y+1)
        workdata->editor->top_row=workdata->editor->y-scrmode->rows+1;
    if (workdata->editor->top_row > workdata->editor->y)
        workdata->editor->top_row=workdata->editor->y;
    if (workdata->editor->top_row+scrmode->rows > rows)
      workdata->editor->top_row=rows-scrmode->rows;
    if (workdata->editor->top_row < 0)
      workdata->editor->top_row=0;
}

/*
 * Action function - start the scrpt mode.
 */
short start_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    workdata->editor->prevmode=scrmode->mode;
    workdata->editor->y=0;
    workdata->editor->top_row=0;
    workdata->editor->err_row=-1;
    workdata->editor->err_param=-9;
    workdata->editor->script=&(workdata->lvl->script);
    scrmode->mode=MD_SCRP;
    return true;
}

/*
 * Action function - end the scrpt mode.
 */
void end_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    workdata->editor->y=0;
    workdata->editor->err_row=-1;
    workdata->editor->err_param=-9;
    workdata->editor->script=NULL;
    if (scrmode->mode!=workdata->editor->prevmode)
      scrmode->mode=workdata->editor->prevmode;
    else
      scrmode->mode=MD_SLB;
}

/*
 * Draws screen for the scrpt mode.
 */
void draw_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    int i;
    int rows;
    rows=workdata->editor->script->lines_count;
    char wordtxt[LINEMSG_SIZE];
    for (i=0; i < scrmode->rows; i++)
    {
      char *txt_line="";
      long line_num=workdata->editor->top_row+i;
      if (line_num < rows)
          txt_line=workdata->editor->script->txt[line_num];
      set_cursor_pos(i,0);
      const struct TXTED_COLORS *ccolor;
      if (line_num==workdata->editor->y)
      {
        ccolor=&script_hl_colors;
      } else
      {
        ccolor=&script_std_colors;
      }
      char *prev_end=txt_line;
      int param_idx=-1;
      char *ptr;
      unsigned int ptr_len;
      short got_word;
      int len;
      got_word=script_strword_pos(&ptr,&ptr_len,txt_line,false);
      while (got_word)
      {
          len=min(ptr-prev_end,LINEMSG_SIZE-1);
          if (len>0)
          {
            if ((line_num==workdata->editor->err_row)&&(workdata->editor->err_param==-2))
              screen_setcolor(ccolor->bad);
            else
              screen_setcolor(ccolor->spaces);
            strncpy(wordtxt,prev_end,len);
            wordtxt[len]='\0';
            screen_printf("%s",wordtxt);
          }
          if ((line_num==workdata->editor->err_row)&&((param_idx==workdata->editor->err_param)||(workdata->editor->err_param<-8)))
            screen_setcolor(ccolor->bad);
          else
          if (param_idx==-1)
            screen_setcolor(ccolor->command);
          else
            screen_setcolor(ccolor->param_text);
          len=min(ptr_len,LINEMSG_SIZE-1);
          strncpy(wordtxt,ptr,len);
          wordtxt[len]='\0';
          screen_printf("%s",wordtxt);
          prev_end=ptr+ptr_len;
          got_word=script_strword_pos(&ptr,&ptr_len,NULL,false);
          param_idx++;
      }
      len=min(strlen(prev_end),LINEMSG_SIZE-1);
      if (len>0)
      {
        if ((line_num==workdata->editor->err_row)&&(workdata->editor->err_param==-2))
          screen_setcolor(ccolor->bad);
        else
          screen_setcolor(ccolor->spaces);
        strncpy(wordtxt,prev_end,len);
        wordtxt[len]='\0';
        screen_printf("%s",wordtxt);
      }
      screen_setcolor(ccolor->comment);
      screen_printf_toeol("");
    }
    set_cursor_pos(get_screen_rows()-1, get_screen_cols()-1);
}

void script_verify_with_highlight(const struct LEVEL *lvl,struct TXTED_DATA *editor)
{
    message_log(" script_verify_with_highlight: starting");
    char err_msg[LINEMSG_SIZE];
    strcpy(err_msg,"Unknown error");
    short result;
    int err_line=-1;
    int err_param=-9;
    result=dkscript_verify(lvl,err_msg,&err_line,&err_param);
    switch (result)
    {
      case VERIF_OK:
        message_info("Script verification passed.");
        editor->err_row=-1;
        editor->err_param=-9;
        break;
      case VERIF_WARN:
        message_error("Warning: %s",err_msg);
        editor->err_row=err_line;
        editor->err_param=err_param;
        break;
      default:
        message_error("Error: %s",err_msg);
        editor->err_row=err_line;
        editor->err_param=err_param;
        break;
    }
    message_log(" script_verify_with_highlight: finished");
}
