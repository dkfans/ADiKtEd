/*
 * main.c   contains main() and initialization functions
 */

#include "globals.h"
#include "scr_actn.h"
#include "lev_data.h"
#include "scr_help.h"
#include "var_utils.h"
#include "input_kb.h"
#include "draw_map.h"
#include "output_scr.h"

const char config_filename[]="map.ini";

void read_init(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
#if defined(unix) && !defined(GO32)
    levels_path="."SEPARATOR"levels";
#else
    levels_path="."SEPARATOR"levels";
#endif
    data_path="."SEPARATOR"data";
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
      int spacepos=strlen(buffer)-1;
      while (spacepos>0)
      {
          if ((buffer[spacepos]==' ')||(buffer[spacepos]=='\t')||(buffer[spacepos]=='\n'))
          {
            buffer[spacepos]='\0';
            spacepos--;
          } else
          {
            spacepos=-1;
          }
      }
      spacepos=strlen(p)-1;
      while (spacepos>0)
      {
          if ((p[spacepos]==' ')||(p[spacepos]=='\t')||(p[spacepos]=='\n'))
          {
            p[spacepos]='\0';
            spacepos--;
          } else
          {
            spacepos=-1;
          }
      }
      if (!strcmp(buffer, "SHOW_OBJ_RANGE"))
      {
          mapmode->show_obj_range=atoi(p);
      } else
      if (!strcmp(buffer, "DAT_VIEW_MODE"))
      {
          mapmode->dat_view_mode=atoi(p);
      } else
      if (!strcmp(buffer, "BITMAP_SCALE"))
      {
          bitmap_rescale=atoi(p);
      } else
      if (!strcmp(buffer, "LEVELS_PATH"))
      {
          levels_path=strdup(p);
          l = strlen(levels_path);
          if (l)
            if (levels_path[l-1]==SEPARATOR[0])
                levels_path[l-1]=0;
      } else
      if (!strcmp(buffer, "DATA_PATH"))
      {
          data_path=strdup(p);
          l = strlen(data_path);
          if (l)
            if (data_path[l-1]==SEPARATOR[0])
                data_path[l-1]=0;
      }
    }
}

void get_command_line_options(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int argc, char **argv)
{
// Note: Automated commands are now just keys inserted into keyboard input buffer.
// in the future, this should be a script generated using input parameters
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
          scrmode->automated_commands[cmnds_count]='v';
          cmnds_count++;
        } else
        if ((strcmp(comnd+1,"q")==0)||(strcmp(comnd+1,"Q")==0))
        {
          scrmode->automated_commands[cmnds_count]=KEY_CTRL_Q;
          cmnds_count++;
        } else
        if (strcmp(comnd+1,"r")==0)
        {
          scrmode->automated_commands[cmnds_count]=KEY_CTRL_R;
          cmnds_count++;
        } else
        if (strcmp(comnd+1,"n")==0)
        {
          scrmode->automated_commands[cmnds_count]=KEY_CTRL_N;
          cmnds_count++;
        } else
        if (strcmp(comnd+1,"ds")==0)
        {
          disable_sounds=true;
        } else
        if (strcmp(comnd+1,"du")==0)
        {
          datclm_auto_update=false;
          obj_auto_update=false;
        } else
        if (strcmp(comnd+1,"dvid")==0)
        {
          scrmode->screen_enabled=false;
        } else
        if (strcmp(comnd+1,"dinp")==0)
        {
          scrmode->input_enabled=false;
        } else
        if (strcmp(comnd+1,"s")==0)
        {
          scrmode->automated_commands[cmnds_count]=KEY_F5;
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
    struct LEVEL *lvl;
    //Allocate and set basic configuration variables
    struct SCRMODE_DATA *scrmode;
    struct MAPMODE_DATA *mapmode;
    init_levscr_basics(&scrmode,&mapmode);
    // Initialize the message displaying and storing
    init_messages();
    // create object for storing map
    level_init(&lvl);
    drawdata.scrmode=scrmode;
    drawdata.mapmode=mapmode;
    drawdata.lvl=lvl;
    // read configuration file
    read_init(scrmode,mapmode);
    // Interpreting command line parameters
    get_command_line_options(scrmode,mapmode,lvl,argc,argv);
    // initing keyboard input and screen output, also all modes.
    init_levscr_modes(scrmode,mapmode);
    // Load a map, or start new one
    if (strlen(lvl->fname)>0)
    {
      popup_show("Loading map","Reading map files. Please wait...");
      load_map(lvl);
    } else
    {
      start_new_map(lvl);
    }
    do 
    {
      draw_levscr(scrmode,mapmode,lvl);
      proc_key(scrmode,mapmode,lvl);
    } while (!finished);
    done(&scrmode,&mapmode,&lvl);
    return 0;
}
