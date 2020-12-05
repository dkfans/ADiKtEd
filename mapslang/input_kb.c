/******************************************************************************/
// input_kb.c - Platform independence assurance module.
/******************************************************************************/
// Author:  Tomasz Lis
// Created: 12 Dec 2007

// Purpose:
//   Defines functions for initializing and reading input from keyboard.
//   This module provides the interface to keyboard input for whole program.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "input_kb.h"

#include <slang.h>
#include "output_scr.h"
#include "var_utils.h"
#include "../libadikted/adikted.h"

short input_initied=false;
short disable_sounds=false;

/*
 * Function for retrieving single key from keyboard.
 */
unsigned int get_key(void)
{
    unsigned int ret;
#if defined(unix) && !defined(GO32)
    if (update_required)
      screen_reinit_and_update();
    safe_update = true;
#endif
    ret = SLkp_getkey();
#if defined(unix) && !defined(GO32)
    safe_update = false;
#endif
#if defined(WIN32)
    if (ret==ABORT_KEY)
    {
      SLang_reset_tty();
      if (SLang_init_tty(ABORT_KEY,1,0) == -1) 
      {
          die("get_key: SLang_init_tty: returned error code");
      }
    }
#endif
    return ret;
}

/*
 * Get a string, in the "minibuffer". Return true on success, false
 * on break. Possibly syntax-highlight the entered string for
 * backslash-escapes, depending on the "highlight" parameter.
 */
int get_str(char *prompt, char *buf) 
{
    int maxlen = LINEMSG_SIZE - strlen(prompt) - 1;  // limit to LINEMSG_SIZE
    int len = 0;
    int c;

    for (;;) 
    {
      SLsmg_gotorc (SLtt_Screen_Rows-1, 0);
      SLsmg_set_color (0);
      SLsmg_write_string (prompt);
      SLsmg_write_nchars (buf, len);

      SLsmg_set_color(0);
      SLsmg_erase_eol();
      SLsmg_refresh();
#if defined(unix) && !defined(GO32)
      if (update_required)
          screen_reinit_and_update();
      safe_update = true;
#endif
      c = SLang_getkey();
#if defined(unix) && !defined(GO32)
      safe_update = false;
#endif
      if ((c == KEY_ENTER)&&(len>0))
      {
          buf[len] = '\0';
          return true;
      } 
      else if ((c == KEY_ESCAPE) || (c == KEY_CTRL_G))
      {
        message_error("User break of input");
        SLKeyBoard_Quit=0;
        SLang_restart(1);
        return false;
      }
      
      if ((c>=32) && (c<=126))
      {
          if (len < maxlen)
            buf[len++] = c;
          else
            speaker_beep();
      }

      if ( ((c==KEY_BACKSP)||(c==KEY_DEL)) && (len>0) )
          len--;

      if (c == 'U'-'@')             // ^U kill line
          len = 0;
    }
}

/*
 * Initialise stuff at the beginning of the program: mostly the
 * SLang library terminal management.
 */
short input_init(void)
{
    SLtt_get_terminfo();

   // SLkp_init assumes that SLtt_get_terminfo has been called.
   if (SLkp_init() == -1)
     {
        die("axe: SLkp_init: returned error code");
     }

    //SLang_init_tty initializes terminal for reading single characters
    //definition: int SLang_init_tty (int abort_char, int flow_ctrl, int opost)
    if (SLang_init_tty(ABORT_KEY,1,0) == -1) 
    {
        die("axe: SLang_init_tty: returned error code");
    }
    SLang_set_abort_signal(NULL);
    //This could allow reading CTRL+key sequences
//    SLgetkey_map_to_ansi(1);
    SLang_flush_input(); 
    input_initied=true;
    return ERR_KB_NONE;
}

/*
 * Clean up all the stuff that input_init() did.
 */
short input_done(void) 
{
    if (!input_initied) return ERR_KB_ALREADY;
    SLang_reset_tty();
    input_initied=false;
    return ERR_KB_NONE;
}

unsigned char key_to_ascii(int key)
{
  return (unsigned char)key;
}

/*
 * Just makes "beep" on error. Disable the body if you require some silence.
 */
void speaker_beep(void)
{
    if (disable_sounds) return;
    SLtt_beep();
}

