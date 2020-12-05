/******************************************************************************/
// scr_txted.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  27 Jan 2008

// Purpose:
//   Defines functions for initializing and displaying the scrpt screen.
//   This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "scr_txted.h"

#include "../libadikted/adikted.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "scr_txtgen.h"
#include "textmenu.h"
#include "var_utils.h"

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
    workdata->editor->word_wrap=true;
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
    message_release();
    if ((workdata->editor->gen_flags&SGF_ENABLED)==SGF_ENABLED)
    {
        if (action_menukey(workdata,workdata->txtgen->currmnu,key))
            return;
        switch (key)
        {
        case KEY_ESCAPE:
            end_scrptgen(scrmode,workdata);
            break;
        }
        return;
    }
    int lines_count;
    int line_y=workdata->editor->y;
    lines_count=workdata->editor->script->lines_count;
    switch (key)
    {
    case KEY_UP:
      line_y--;
      break;
    case KEY_DOWN:
      line_y++;
      break;
    case KEY_PGUP:
      line_y-=scrmode->rows-1;
      break;
    case KEY_PGDOWN:
      line_y+=scrmode->rows-1;
      break;
    case KEY_HOME:
      line_y=0;
      break;
    case KEY_END:
      line_y=lines_count-1;
      break;

    case KEY_U:
      {
        struct DK_SCRIPT *scrpt=get_lvl_script(workdata->lvl);
        recompute_script_levels(scrpt);
        short retcode=recompose_script(scrpt,&(workdata->optns->script));
        if (retcode)
          message_info("Script recomposed successfully");
        else
          message_error("Error occured during the update");
      };break;
        case KEY_V: // Verify whole map
          script_verify_with_highlight(workdata->lvl,workdata->editor);
          break;
    case KEY_TAB:
    case KEY_ESCAPE:
      end_scrpt(scrmode,workdata);
      message_info("Returned to last work mode");
      break;
    default:
      message_info("Unrecognized script editor key code: %d",key);
      speaker_beep();
    }
    //Fixing y
    if (line_y >= lines_count)
      line_y=lines_count-1;
    if (line_y < 0)
      line_y=0;
    workdata->editor->y=line_y;
    // Finding which row we need to see for the command at y to be visible
    int rows=workdata->editor->total_rows_count;
    int curr_row=0;
    int curr_count;
    int i;
    for (i=0;i<line_y;i++)
        curr_row+=workdata->editor->line_rows[i];
    curr_count=workdata->editor->line_rows[line_y];
    //Fixing top_row based on y
    if (workdata->editor->top_row+scrmode->rows < curr_row+curr_count)
        workdata->editor->top_row=curr_row-scrmode->rows+curr_count;
    if (workdata->editor->top_row > curr_row)
        workdata->editor->top_row=curr_row;
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
    if (workdata==NULL) return false;
    struct DK_SCRIPT *script=get_lvl_script(workdata->lvl);
    if (script==NULL) return false;
    workdata->editor->prevmode=scrmode->mode;
    workdata->editor->y=0;
    workdata->editor->top_row=0;
    workdata->editor->err_row=-1;
    workdata->editor->err_param=ERR_SCRIPTPARAM_WHOLE;
    workdata->editor->script=script;
    workdata->editor->line_rows=NULL;
    if (!recompute_editor_lines(workdata->editor,script,get_screen_cols()))
        return false;
    scrmode->mode=MD_SCRP;
    inc_info_usr_mdswtch_count(workdata->lvl);
    return true;
}

short recompute_editor_lines(struct TXTED_DATA *editor,const struct DK_SCRIPT *script,
    const unsigned int scr_width)
{
    free(editor->line_rows);
//    message_log("recompute_editor_lines: starting");
    editor->line_rows=(int *)malloc((script->lines_count+1)*sizeof(unsigned int));
    if (editor->line_rows==NULL) return false;
    editor->scr_width=scr_width;
    unsigned int all_lines=script->lines_count;
    unsigned int total_rows=0;
    int line_num;
    for (line_num=0;line_num<all_lines;line_num++)
    {
        char *txt_line;
        txt_line=editor->script->txt[line_num];
        int ln_rows=1;
// Note: the word wrap works only if '\t' is converted into one space.
// this is because there's no proper strlen function which counts tab as
// more than 1 char
        if (editor->word_wrap)
        {
          if (strlen(txt_line)>scr_width)
            ln_rows++;
        }
        editor->line_rows[line_num]=ln_rows;
        total_rows+=ln_rows;
    }
    message_log("recompute_editor_lines: got %d screen lines from %d script lines",
        total_rows,all_lines);
    editor->line_rows[all_lines]=0;
    editor->total_rows_count=total_rows;
    return true;
}

/*
 * Action function - end the scrpt mode.
 */
void end_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    workdata->editor->y=0;
    workdata->editor->err_row=-1;
    workdata->editor->err_param=ERR_SCRIPTPARAM_WHOLE;
    workdata->editor->script=NULL;
    free(workdata->editor->line_rows);
    workdata->editor->total_rows_count=0;
    if (scrmode->mode!=workdata->editor->prevmode)
      scrmode->mode=workdata->editor->prevmode;
    else
      scrmode->mode=MD_SLB;
}

/*
 * Works like strncpy, but replaces any control characters with '_' or spaces.
 * Always ends the destination with '\0' char. Returns number of characters
 * processed from source string (it may be different that destination length!).
 * Replaces '\t' with constant amount of spaces given in tab_len.
 */
int strncpy_skipctrl(char *destination, const char *source, size_t num, const int tab_len)
{
    if (destination==NULL) return 0;
    if (source==NULL)
    {
      destination[0]='\0';
      return 0;
    }
    int src_idx=0;
    int dst_idx=0;
    int i;
    while (dst_idx<num)
    {
      char chr=source[src_idx];
      if (chr=='\0') break;
      if (chr=='\t')
      {
        for (i=0;i<tab_len;i++)
        {
          destination[dst_idx]=' ';
          dst_idx++;
        }
      } else
      if ((unsigned char)chr<(unsigned char)' ')
      {
        destination[dst_idx]='_';
        dst_idx++;
      } else
      {
        destination[dst_idx]=chr;
        dst_idx++;
      }
      src_idx++;
    }
    destination[dst_idx]='\0';
    return src_idx;
}

/*
 * Draws screen for the script mode.
 */
void draw_scrpt(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    int i;
    int lines_count;
    lines_count=workdata->editor->script->lines_count;
    int top_row;
    top_row=workdata->editor->top_row;
    long line_num=0;
    long curr_row=0;
    long line_rows;
    if (lines_count>line_num)
      line_rows=workdata->editor->line_rows[line_num];
    else
      line_rows=1;
    while ((curr_row+line_rows) <= top_row)
    {
      curr_row+=line_rows;
      line_num++;
      if (line_num>=lines_count)
        break;
      line_rows=workdata->editor->line_rows[line_num];
    }
    int curr_lnrow=top_row-curr_row;
    if (curr_lnrow<0) curr_lnrow=0;
    // Looping through screen lines
    char wordtxt[LINEMSG_SIZE];
    short new_line=true;
    char *txt_line;
    const struct TXTED_COLORS *ccolor;
    short got_word;
    char const *prev_end;
    int param_idx;
    char const *ptr;
    unsigned int ptr_len;
    int max_param_len;
    for (i=0; i < scrmode->rows; i++)
    {
      set_cursor_pos(i,0);
      if (new_line)
      {
        if (line_num < lines_count)
            txt_line=workdata->editor->script->txt[line_num];
        else
            txt_line="";
        if (((workdata->editor->gen_flags&SGF_ENABLED)!=SGF_ENABLED) &&
           (line_num==workdata->editor->y))
            ccolor=&script_hl_colors;
        else
            ccolor=&script_std_colors;
        got_word=script_strword_pos(&ptr,&ptr_len,txt_line,false);
        prev_end=txt_line;
        param_idx=-1;
        max_param_len=min(workdata->editor->scr_width,LINEMSG_SIZE-1);
        new_line=false;
      } else
      {
          int remain_len;
          if (ptr<prev_end)
            remain_len=strlen(ptr);
          else
            remain_len=strlen(prev_end);
          while (max_param_len>(remain_len+1))
          {
            screen_printf(" ");
            max_param_len--;
          }
      }
      int len;
      while (got_word)
      {
          //Drawing free spaces between parameters
          int word_len;
          word_len=ptr-prev_end;
          len=min(word_len,max_param_len);
          if (len>0)
          {
            if ((line_num==workdata->editor->err_row) &&
               (workdata->editor->err_param==ERR_SCRIPTPARAM_NARGS))
              screen_setcolor(ccolor->bad);
            else
              screen_setcolor(ccolor->spaces);
            int real_len=strncpy_skipctrl(wordtxt,prev_end,len,1);
            screen_printf("%s",wordtxt);
            max_param_len-=real_len;
            prev_end+=len;
          }
          if (word_len>len)
            break;
          if ((line_num==workdata->editor->err_row) &&
             ((param_idx==workdata->editor->err_param) ||
              (workdata->editor->err_param<=ERR_SCRIPTPARAM_WHOLE)))
            screen_setcolor(ccolor->bad);
          else
          if (param_idx==-1)
            screen_setcolor(ccolor->command);
          else
            screen_setcolor(ccolor->param_text);
          //Drawing the parameter
          len=min(ptr_len,max_param_len);
          if (len>0)
          {
            // If there is end of a word near, and another line will be drawn,
            // then break the line there.
            if ((ptr_len>max_param_len)&&((curr_lnrow+1)<line_rows))
            {
              int max_back=ptr_len-1;
              if (max_back>8) max_back=8;
              int i;
              for (i=1;i<max_back;i++)
                if ((unsigned char)ptr[len-i]<=(unsigned char)' ')
                {
                  len=len+1-i;
                  break;
                }
            }
            int real_len=strncpy_skipctrl(wordtxt,ptr,len,1);
            screen_printf("%s",wordtxt);
            max_param_len-=real_len;
          }
          if (ptr_len>len)
          {
            //Remove printed length from parameter length
            ptr+=len;
            ptr_len-=len;
            //Setting this will skip first part of the loop (drawing free spaces)
            prev_end=ptr;
            break;
          } else
          {
            prev_end=ptr+len;
            got_word=script_strword_pos(&ptr,&ptr_len,NULL,false);
            param_idx++;
          }
      }
      if (got_word)
      {
          max_param_len=min(workdata->editor->scr_width,LINEMSG_SIZE-1);
      } else
      {
        len=min(strlen(prev_end),LINEMSG_SIZE-1);
        if (len>0)
        {
          if ((line_num==workdata->editor->err_row) &&
             (workdata->editor->err_param==ERR_SCRIPTPARAM_NARGS))
            screen_setcolor(ccolor->bad);
          else
            screen_setcolor(ccolor->spaces);
          strncpy_skipctrl(wordtxt,prev_end,len,1);
          screen_printf("%s",wordtxt);
        }
      }
      screen_setcolor(ccolor->comment);
      screen_printf_toeol("");
      curr_lnrow++;
      if (curr_lnrow>=line_rows)
      {
        line_num++;
        if (line_num < lines_count)
          line_rows=workdata->editor->line_rows[line_num];
        else
          line_rows=1;
        curr_lnrow=0;
        new_line=true;
      }
    }
    if ((workdata->editor->gen_flags&SGF_ENABLED)==SGF_ENABLED)
    {
      draw_menuscr(workdata->txtgen->currmnu,scrmode->rows,scrmode->cols);
      // Place cursor appropriately
      draw_menu_cursor(workdata->txtgen->currmnu,scrmode->rows,scrmode->cols);
    } else
    {
      show_no_cursor();
    }
}

void script_verify_with_highlight(const struct LEVEL *lvl,struct TXTED_DATA *editor)
{
    message_log(" script_verify_with_highlight: starting");
    char err_msg[LINEMSG_SIZE];
    strcpy(err_msg,"Unknown error");
    short result;
    int err_line=-1;
    int err_param=ERR_SCRIPTPARAM_WHOLE;
    result=dkscript_verify(lvl,err_msg,&err_line,&err_param);
    switch (result)
    {
      case VERIF_OK:
        message_info("Script verification passed.");
        editor->err_row=-1;
        editor->err_param=ERR_SCRIPTPARAM_WHOLE;
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
