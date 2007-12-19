/*
 * input_kb.c
 *
 * Defines functions for initializing and reading input from keyboard.
 * This module provides the interface to keyboard input for whole program.
 *
 */

#include "input_kb.h"

#include "globals.h"
#include <slang.h>

short input_initied=false;
short disable_sounds=false;

/*
 * Function for retrieving single key from keyboard.
 */
unsigned int get_key (void)
{
    unsigned int ret;
#if defined(unix) && !defined(GO32)
    if (update_required)
      update();
    safe_update = true;
#endif
    ret = SLkp_getkey();
#if defined(unix) && !defined(GO32)
    safe_update = FALSE;
#endif
    return ret;
}

/*
 * Get a string, in the "minibuffer". Return TRUE on success, FALSE
 * on break. Possibly syntax-highlight the entered string for
 * backslash-escapes, depending on the "highlight" parameter.
 */
int get_str (char *prompt, char *buf) 
{
    int maxlen = 79 - strlen(prompt);  // limit to 80 - who cares? :)
    int len = 0;
    int c;

    for (;;) 
    {
      SLsmg_gotorc (SLtt_Screen_Rows-1, 0);
      SLsmg_set_color (0);
      SLsmg_write_string (prompt);
      SLsmg_write_nchars (buf, len);

      SLsmg_set_color (0);
      SLsmg_erase_eol();
      SLsmg_refresh();
#if defined(unix) && !defined(GO32)
      if (update_required)
          update();
      safe_update = TRUE;
#endif
      c = SLang_getkey();
#if defined(unix) && !defined(GO32)
      safe_update = FALSE;
#endif
      if ((c == KEY_ENTER)&&(len>0))
      {
          buf[len] = '\0';
          return true;
      } 
      else if ((c == KEY_ESCAPE) || (c == 7))
      {
        message_error("User break of input");
          SLKeyBoard_Quit=0;
        SLang_set_error(0);
          return false;
      }
      
      if (c >= 32 && c <= 126)
      {
          if (len < maxlen)
            buf[len++] = c;
          else
            speaker_beep();
      }

      if (((c == 127) || (c == KEY_BACKSP)) && len > 0)
          len--;

      if (c == 'U'-'@')             /* ^U kill line */
          len = 0;
    }
}

/*
 * Initialise stuff at the beginning of the program: mostly the
 * SLang library terminal management.
 */
void input_init(void)
{
    SLtt_get_terminfo();

   // SLkp_init assumes that SLtt_get_terminfo has been called.
   if (SLkp_init() == -1)
     {
        fprintf(stderr, "axe: SLkp_init: returned error code\n");
        exit(1);
     }

    //SLang_init_tty initializes terminal for reading single characters
    //definition: int SLang_init_tty (int abort_char, int flow_ctrl, int opost)
    if (SLang_init_tty(ABORT_KEY,1,0) == -1) 
    {
        fprintf(stderr, "axe: SLang_init_tty: returned error code\n");
        exit(1);
    }
    SLang_set_abort_signal(NULL);
    //This could allow reading CTRL+key sequences
//    SLgetkey_map_to_ansi(1);
    SLang_flush_input(); 
    input_initied=true;
}

/*
 * Clean up all the stuff that input_init() did.
 */
void input_done(void) 
{
    if (!input_initied) return;
    SLang_reset_tty();
    input_initied=false;
}

/*
 * Just makes "beep" on error. Disable the body if you require some silence.
 */
void speaker_beep()
{
    if (disable_sounds) return;
    SLtt_beep();
}

