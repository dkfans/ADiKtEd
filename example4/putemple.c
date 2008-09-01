/******************************************************************************/
/** @file putemple.c
 * ADiKtEd library example 4.
 * @par Purpose:
 *     Demonstrates fast drawing routines and putting slabs with mouse.
 *     Also, shows how to draw ADiKtEd messages using DK font.
 *     Use left/right arrows to switch levels, numpad +/- to zoom,
 *     LMB to move, RMB to put temple tiles. You can't save.
 * @par Comment:
 *     The program will require library SDL.
 *     You have to download it and install before recompiling.
 * @author   Tomasz Lis
 * @date     28 Jul 2008 - 02 Jun 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#include "../libadikted/adikted.h"

#define ONE_LOOP_DELAY 20
/**
 * The screen surface.
 */
SDL_Surface *screen = NULL;

/**
 * Files required to draw map are loaded into this struct.
 */
struct MAPDRAW_DATA *draw_data=NULL;

/**
 * Bitmap surface - a buffer between level and screen.
 */
SDL_Surface *bitmap=NULL;

/**
 * Top left corner of the drawn area.
 */
struct IPOINT_2D startpt;

/**
 * Size of the whole map area for current scale.
 */
struct IPOINT_2D map_size;

/**
 * Source level.
 */
struct LEVEL *lvl;

/**
 * Level index.
 */
static int lvl_idx=11;

/**
 * Finish program flag.
 */
static short done=0;

/**
 * Loads files required to draw map.
 * Both normal and fast drawing require same code here.
 */
short prepare_map_drawing(struct LEVEL *lvl, int width, int height)
{
    short result;
    struct MAPDRAW_OPTIONS *opts=level_get_mapdraw_options(lvl);
    // Free any previous data (if the function was called before)
    free_draw_data(draw_data);
    SDL_FreeSurface(bitmap);
    // Texture and bitmap size
    struct IPOINT_2D bmp_size;
    // Settings to draw whole map
    get_full_draw_dimensions_opt(&map_size,&(lvl->subsize),opts);
    message_log("Map dimensions %d,%d.",map_size.x,map_size.y);
    if (map_size.x>width)
        bmp_size.x=width+1;
    else
        bmp_size.x=map_size.x;
    if (map_size.y>height)
        bmp_size.y=height+1;
    else
        bmp_size.y=map_size.y;
    message_log("Bitmap dimensions %d,%d.",bmp_size.x,bmp_size.y);
    // Load the data files
    result=load_draw_data(&draw_data,opts,&(lvl->subsize),bmp_size,get_lvl_inf(lvl));
    if (result==ERR_NONE)
    {
      // Create bitmap surface to draw on
      bitmap=SDL_CreateRGBSurface(SDL_SWSURFACE,bmp_size.x,bmp_size.y,24,0x00ff0000,0x000000ff,0x0000ff00,0);
      if (bitmap==NULL)
      {
        message_error("prepare_map_drawing: Cannot create bitmap surface.");
        free_draw_data(draw_data);
        result=2;
      }
      set_draw_data_rect(draw_data,startpt.x,startpt.y,
          startpt.x+bmp_size.x-1,startpt.y+bmp_size.y-1,bitmap->pitch,opts->rescale);
    }
    return result;
}

/**
 * This function draws to the screen.
 */
static void draw_screen()
{
/*
    // For some graphics modes, you will have to make conversion
    // before blit operation, like this:
    SDL_Surface *tmp = SDL_DisplayFormat(bitmap);
    SDL_BlitSurface(tmp, NULL, screen, NULL);
    SDL_FreeSurface(tmp);
*/
    if (screen->w > bitmap->w)
        SDL_FillRect(screen,NULL,0);

    // Blit the bitmap directly to screen
    SDL_BlitSurface(bitmap, NULL, screen, NULL);

    // Make sure everything is displayed on screen
    SDL_Flip (screen);
}

/**
 * This function processes keyboard/mouse events.
 */
static void process_events()
{
    static struct IPOINT_2D viewmove={0,0};
    static short clip_view=0;
    static short reload_data=1;
    static short reload_lvl=1;
    static short level_redraw=1;
    static short slab_drawing = -1;
    static struct IPOINT_2D mouse_movestart={-1,0};

    SDL_Event event;

    // Check for events
    while (SDL_PollEvent(&event))
    {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_PLUS:
                case SDLK_KP_PLUS:
                    {
                        struct MAPDRAW_OPTIONS *opts=level_get_mapdraw_options(lvl);
                        if (opts->rescale > 0)
                        {
                            opts->rescale--;
                            startpt.x = (startpt.x<<1) + (bitmap->w/2);
                            startpt.y = (startpt.y<<1) + (bitmap->h/2);
                            clip_view=1;
                            reload_data=1;
                        } else
                        {
                            message_error("Scale factor minimum reached");
                            level_redraw=1;
                        }
                    };break;
                case SDLK_MINUS:
                case SDLK_KP_MINUS:
                    {
                        struct MAPDRAW_OPTIONS *opts=level_get_mapdraw_options(lvl);
                        if (opts->rescale < 5)
                        {
                            opts->rescale++;
                            startpt.x = (startpt.x>>1) - (bitmap->w/4);
                            startpt.y = (startpt.y>>1) - (bitmap->h/4);
                            clip_view=1;
                            reload_data=1;
                        } else
                        {
                            message_error("Maxumum scale factor reached");
                            level_redraw=1;
                        }
                    };break;
                case SDLK_RIGHT:
                    lvl_idx%=20;
                    lvl_idx+=1;
                    reload_lvl=1;
                    break;
                case SDLK_LEFT:
                    lvl_idx--;
                    if (lvl_idx<=0) lvl_idx=20;
                    reload_lvl=1;
                    break;
                case SDLK_DOWN:
                    viewmove.y=1;
                    break;
                case SDLK_UP:
                    viewmove.y=-1;
                    break;
                case SDLK_END:
                    viewmove.x=16;
                    break;
                case SDLK_HOME:
                    viewmove.x=-16;
                    break;
                case SDLK_PAGEDOWN:
                    viewmove.y=16;
                    break;
                case SDLK_PAGEUP:
                    viewmove.y=-16;
                    break;
                case SDLK_ESCAPE:
                    done = 1;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_END:
                case SDLK_HOME:
                    viewmove.x=0;
                    break;
                case SDLK_DOWN:
                case SDLK_UP:
                case SDLK_PAGEDOWN:
                case SDLK_PAGEUP:
                    viewmove.y=0;
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    {
                      short obj_reslt;
                      unsigned int sx,sy,z;
                      obj_reslt=get_object_with_circle_at(&sx,&sy,&z,draw_data,lvl,
                          event.button.x,event.button.y);
                      if (obj_reslt==ERR_NONE)
                      {
                          unsigned char *obj=get_object(lvl,sx,sy,z);
                          char *obj_name;
                          switch (get_object_type(lvl,sx,sy,z))
                          {
                          case OBJECT_TYPE_THING:
                              obj_name=get_thing_subtype_fullname(get_thing_type(obj),get_thing_subtype(obj));
                              break;
                          case OBJECT_TYPE_ACTNPT:
                              obj_name="Action point";
                              break;
                          case OBJECT_TYPE_STLIGHT:
                              obj_name="Static light";
                              break;
                          default:
                              obj_name="Unknown object";
                              break;
                          }
                          message_info("%s at (%d,%d)",obj_name,sx,sy);
                          level_redraw=1;
                      } else
                      {
                          mouse_movestart.x=event.button.x;
                          mouse_movestart.y=event.button.y;
                      }
                    }
                    break;
                case SDL_BUTTON_RIGHT:
                    {
                      slab_drawing=SLAB_TYPE_TEMPLE;
                      unsigned int sx,sy;
                      get_subtile_from_draw_coords(&sx,&sy,draw_data,
                          event.button.x,event.button.y);
                      unsigned int tx,ty;
                      tx=sx/MAP_SUBNUM_X;ty=sy/MAP_SUBNUM_Y;
                      if (get_tile_slab(lvl,tx,ty)!=slab_drawing)
                      {
                          user_set_slab(lvl,tx,ty,slab_drawing);
                          user_set_tile_owner(lvl,tx,ty,PLAYER0);
                      }
                      message_info("New %s put at (%d,%d)",
                          get_slab_fullname(get_tile_slab(lvl,tx,ty)),tx,ty);
                      level_redraw=1;
                    }
                    break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    mouse_movestart.x=-1;
                    break;
                case SDL_BUTTON_RIGHT:
                    slab_drawing=-1;
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (mouse_movestart.x>=0)
                {
                      startpt.x+=mouse_movestart.x-event.motion.x;
                      startpt.y+=mouse_movestart.y-event.motion.y;
                      mouse_movestart.x=event.motion.x;
                      mouse_movestart.y=event.motion.y;
                      clip_view=1;
                }
                if (slab_drawing>=0)
                {
                      unsigned int sx,sy;
                      get_subtile_from_draw_coords(&sx,&sy,draw_data,
                          event.motion.x-event.motion.xrel,event.motion.y-event.motion.yrel);
                      struct IPOINT_2D tile_prv;
                      tile_prv.x=sx/MAP_SUBNUM_X;
                      tile_prv.y=sy/MAP_SUBNUM_Y;
                      get_subtile_from_draw_coords(&sx,&sy,draw_data,
                          event.motion.x,event.motion.y);
                      struct IPOINT_2D tile_cur;
                      tile_cur.x=sx/MAP_SUBNUM_X;
                      tile_cur.y=sy/MAP_SUBNUM_Y;
                      int dist_x=(tile_prv.x-tile_cur.x);
                      int dist_y=(tile_prv.y-tile_cur.y);
                      int max_dist=max(abs(dist_x),abs(dist_y));
                      max_dist++;
                      int i;
                      int tx,ty;
                      for (i=0;i<max_dist;i++)
                      {
                          tx=tile_cur.x+(dist_x*i/max_dist);
                          ty=tile_cur.y+(dist_y*i/max_dist);
                          if (get_tile_slab(lvl,tx,ty)!=slab_drawing)
                          {
                            user_set_slab(lvl,tx,ty,slab_drawing);
                            user_set_tile_owner(lvl,tx,ty,PLAYER0);
                          }
                      }
                      if (max_dist==1)
                          message_info("New %s put at (%d,%d)",
                              get_slab_fullname(get_tile_slab(lvl,tx,ty)),tx,ty);
                      else
                          message_info("Changed %d slabs, last at (%d,%d)",max_dist,tx,ty);
                      level_redraw=1;
                }
                break;
            case SDL_VIDEORESIZE: //Screen resized
                screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 24, SDL_SWSURFACE|SDL_RESIZABLE);
                if (screen == NULL)
                {
                  message_error("Couldn't change video mode: %s\n", SDL_GetError());
                  reload_data=0;
                  level_redraw=0;
                  clip_view=0;
                  done = 1;
                  return;
                }
                clip_view=1;
                reload_data=1;
                break;
            case SDL_QUIT:
                done = 1;
                break;
            default:
                break;
            }
      }

      if (viewmove.x)
      {
          startpt.x+=viewmove.x;
          clip_view=1;
      }
      if (viewmove.y)
      {
          startpt.y+=viewmove.y;
          clip_view=1;
      }

      if (reload_lvl)
      {
          // Here we will store map file name
          char level_fname[16];
          sprintf(level_fname,"MAP%05d",lvl_idx);
          // Form a file name of the map to load.
          format_lvl_fname(lvl,level_fname);
          // Sending message into buffer allows it to be logged
          // into file if we use set_msglog_fname() before.
          message_info("loading map file...");
          // Loading map
          short result;
          result=user_load_map(lvl,0);
          if (result!=ERR_NONE)
          {
            // If an error occurs, then the message buffer
            // contains error message.
            // Release the error message.
            message_release();
            message_info("example4 finished with map load error");
            reload_data=0;
            level_redraw=0;
            clip_view=0;
            done = 1;
            return;
          }
          message_info("%s loaded successfully",level_fname);
          reload_lvl=0;
          reload_data=1;
          level_redraw=1;
      }

      if (reload_data)
      {
        short result;
        result=prepare_map_drawing(lvl, screen->w, screen->h);
        if (result!=ERR_NONE)
        {
          // Release the error message.
          message_release();
          message_info("example4 finished with data files load error");
          level_redraw=0;
          clip_view=0;
          done = 1;
          return;
        }
        message_info("Data reloaded for rescale %d",draw_data->rescale);
        reload_data=0;
        level_redraw=1;
      }

      if (clip_view)
      {
        if (startpt.x>map_size.x-screen->w)
            startpt.x=map_size.x-screen->w;
        if (startpt.y>map_size.y-screen->h)
            startpt.y=map_size.y-screen->h;
        if (startpt.x<0) startpt.x=0;
        if (startpt.y<0) startpt.y=0;
        struct MAPDRAW_OPTIONS *opts=level_get_mapdraw_options(lvl);
        set_draw_data_rect(draw_data,startpt.x,startpt.y,
                startpt.x+bitmap->w-1,startpt.y+bitmap->h-1,bitmap->pitch,opts->rescale);
        clip_view=0;
        level_redraw=1;
      }

      if (level_redraw)
      {
          struct MAPDRAW_OPTIONS *opts=level_get_mapdraw_options(lvl);
          // Notice that here we're using fast version of the drawing routine
          SDL_LockSurface(bitmap);
          draw_map_on_buffer_fast(bitmap->pixels,lvl,draw_data,0);
          // If the image is big enough, put things on it
          if ((opts->rescale)<5)
            draw_things_on_buffer(bitmap->pixels,lvl,draw_data);
          draw_text_on_buffer(bitmap->pixels,16,bitmap->h-18,message_get(),draw_data,1);
          draw_text_on_buffer(bitmap->pixels,16,bitmap->h-32,message_get_prev(),draw_data,0);
          message_release();
          SDL_UnlockSurface(bitmap);
          level_redraw=0;
    }
}

int main (int argc, char *argv[])
{
  // Initialize the message displaying and storing
  init_messages();
  // Logging messages into file
  set_msglog_fname("putemple.log");

  message_log("Initializing SDL");
  // Initialize SDL
  if (SDL_Init (SDL_INIT_VIDEO) < 0)
  {
        message_error("Couldn't initialize SDL: %s\n", SDL_GetError());
        free_messages();
        return 1;
  }
  atexit (SDL_Quit);

  // Set the video mode
  screen = SDL_SetVideoMode (512, 512, 24, SDL_SWSURFACE | SDL_RESIZABLE);
  if (screen == NULL)
  {
        message_error("Couldn't set video mode: %s\n", SDL_GetError());
        free_messages();
        return 2;
  }
  SDL_WM_SetCaption ("ADiKtEd Libray example 4", NULL);

  message_log("Preparing data structures");
  // create object for storing map
  level_init(&lvl,MFV_DKGOLD,NULL);
  // Set path to levels
  set_levels_path(lvl,"./Levels");
  // And to data files
  set_data_path(lvl,"./data");
  struct MAPDRAW_OPTIONS *opts=level_get_mapdraw_options(lvl);
  opts->rescale = 3;
  // If you want to use the bitmap fonts, you must set that
  // before creating MAPDRAW_DATA structure!
  opts->bmfonts |= BMFONT_LOAD_SMALL|BMFONT_LOAD_LARGE;
  opts->tngflags |= TNGFLG_SHOW_CIRCLES;

  message_info("entering application loop");

  while (!done)
  {
      // Prepare the time control
      time_t begin_tm;
      time_t end_tm;
      time(&begin_tm);
      // Get input events
      process_events();
      // Draw to screen
      draw_screen();
      // Don't run too fast
      time(&end_tm);
      int delay=(end_tm-begin_tm);
      if (delay<ONE_LOOP_DELAY)
        SDL_Delay(ONE_LOOP_DELAY-delay);
  }

  message_info("application loop terminated");

  // Free the data used for displaying
  free_draw_data(draw_data);

  // Free memory allocated for level
  level_free(lvl);
  level_deinit(&lvl);

  // This command should be always last function used from library
  free_messages();
  return 0;
}
