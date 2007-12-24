/*
 * main.c   contains main() and initialization functions
 */

#include "globals.h"
#include "scr_actn.h"
#include "lev_data.h"
#include "scr_help.h"
#include "var_utils.h"
#include "input_kb.h"

const char config_filename[]="map.ini";

void read_init(void)
{
    // Do something else here
#if defined(unix) && !defined(GO32)
    filebase="levels";
#else
    filebase="levels";
#endif
    char buffer[READ_BUFSIZE];
    char *p;
    int l;
    
    FILE *fp;
    fp = fopen (config_filename, "rb");
    // If we can't get anything, warn but don't die
    if (!fp)
    {
      message_info_force("Couldn't open %s, defaults loaded.",config_filename);
      return;
    } else
    {
      message_info_force("Welcome to Adikted. Press F1 for help.");
    }
    while (fgets(buffer, READ_BUFSIZE-1, fp))
    {
      //Searchinf for a line containing equal sign
      p = strchr (buffer, '=');
      if (p==NULL)
          continue;
      strip_crlf(buffer);
      (*p)=0;
      p++;
      while ((*p==' ')||(*p=='\t')) p++;
      if (strchr (buffer, ' '))
          *strchr (buffer, ' ')=0;
      if (!strcmp(buffer, "dat_view_mode"))
          dat_view_mode=atoi(p);
      if (!strcmp(buffer, "filebase"))
      {
          filebase=strdup(p);
          if (strchr(filebase,' '))
            *strchr(filebase,' ')=0;
          l = strlen (filebase);
          if (l)
            if (filebase[l-1]==SEPARATOR[0])
                filebase[l-1]=0;
      }
    }
}

void get_command_line_options(int argc, char **argv)
{
    int cmnds_count=0;
    short get_msgout_fname=false;
    short get_savout_fname=false;
    int i;
    for (i=1;i<argc;i++)
    {
      char *comnd=argv[i];
      if (strlen(comnd)<1) continue;
      if ((comnd[0]=='-')||(comnd[0]=='/'))
      { //Setting program option
        get_msgout_fname=false;
        get_savout_fname=false;
        if (strcmp(comnd+1,"v")==0)
        {
          automated_commands[cmnds_count]='v';
          cmnds_count++;
        } else
        if ((strcmp(comnd+1,"q")==0)||(strcmp(comnd+1,"Q")==0))
        {
          automated_commands[cmnds_count]=KEY_CTRL_Q;
          cmnds_count++;
        } else
        if (strcmp(comnd+1,"r")==0)
        {
          automated_commands[cmnds_count]=KEY_CTRL_R;
          cmnds_count++;
        } else
        if (strcmp(comnd+1,"n")==0)
        {
          automated_commands[cmnds_count]=KEY_CTRL_N;
          cmnds_count++;
        } else
        if (strcmp(comnd+1,"ds")==0)
        {
          disable_sounds=true;
        } else
        if (strcmp(comnd+1,"dvid")==0)
        {
          screen_enabled=false;
        } else
        if (strcmp(comnd+1,"dinp")==0)
        {
          input_enabled=false;
        } else
        if (strcmp(comnd+1,"s")==0)
        {
          automated_commands[cmnds_count]=KEY_F5;
          cmnds_count++;
          get_savout_fname=true;
        } else
        if (strcmp(comnd+1,"m")==0)
        {
          get_msgout_fname=true;
        } else
        message_info_force("Unrecognized command line option: \"%s\".",comnd);
      } else
      { //Setting map name
        if (get_msgout_fname)
        {
          set_msglog_fname(comnd);
        } else
        if (get_savout_fname)
        {
          format_map_fname(lvl->savfname,comnd);
        } else
        {
          format_map_fname(lvl->fname,comnd);
        }
      }
    }
}


/*
 * Main function; initializes variables, loads map from commandline
 * parameters and enters the main program loop.
 */
int main(int argc, char **argv)
{
    //Allocate and set basic configuration variables
    init_levscr_basics();
    // Initialize the message displaying and storing
    init_messages();
    // create object for storing map
    level_init();
    // read configuration file
    read_init();
    // Interpreting command line parameters
    get_command_line_options(argc,argv);
    // initing keyboard input and screen output, also all modes.
    init_levscr_modes();
    // Load a map, or start new one
    if (strlen(lvl->fname)>0)
    {
      load_map(lvl);
    } else
    {
      start_new_map(lvl);
    }
    do 
    {
      draw_levscr(lvl);
      proc_key();
    } while (!finished);
    done();
    return 0;
}
