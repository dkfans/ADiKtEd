/******************************************************************************/
/** @file viewmap.c
 * ADiKtEd library viewmap example.
 * @par Purpose:
 *     This example shows how to view maps graphically using libAdikted.
 *     Move with mouse, zoom with numpad +/-.
 *     You can maximize the window for better effect.
 * @par Comment:
 *     The program will require library SDL.
 *     You have to download it and install before recompiling.
 * @author   Tomasz Lis
 * @date     28 Jul 2008 - 29 Jul 2008
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

#include "libadikted/adikted.h"

#define ONE_LOOP_DELAY 32
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
 * Finish program flag.
 */
static short done=0;

/**
 * Main loop counter.
 */
static unsigned int loop_count=0;

/**
 * Loads files required to draw map.
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
    message_error("Map dimensions %d,%d.",map_size.x,map_size.y);
    if (map_size.x>width)
        bmp_size.x=width+1;
    else
        bmp_size.x=map_size.x;
    if (map_size.y>height)
        bmp_size.y=height+1;
    else
        bmp_size.y=map_size.y;
    message_error("Bitmap dimensions %d,%d.",bmp_size.x,bmp_size.y);
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
    static int clip_view=0;
    static int reload_data=1;
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
                        }
                    };break;
                case SDLK_RIGHT:
                    viewmove.x=1;
                    break;
                case SDLK_LEFT:
                    viewmove.x=-1;
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
                mouse_movestart.x=event.motion.x;
                mouse_movestart.y=event.motion.y;
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_movestart.x=-1;
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
                break;
            case SDL_VIDEORESIZE: //Screen resized
                screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 24, SDL_SWSURFACE|SDL_RESIZABLE);
                if (screen == NULL)
                {
                  message_error("Couldn't change video mode: %s\n", SDL_GetError());
                  reload_data=0;
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

      if (reload_data)
      {
        short result;
        result=prepare_map_drawing(lvl, screen->w, screen->h);
        if (result!=ERR_NONE)
        {
          // Release the error message.
          message_release();

          message_info("viewmap finished with data files load error");

          clip_view=0;
          done = 1;
          return;
        }
        reload_data=0;
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
      }

      // Redrawing the level bitmap
      {
          struct MAPDRAW_OPTIONS *opts=level_get_mapdraw_options(lvl);
          // Using standard version of the drawing routine
          SDL_LockSurface(bitmap);
          draw_map_on_buffer(bitmap->pixels,lvl,draw_data,loop_count>>2);
          // If the image is big enough, put things on it
          if ((opts->rescale)<5)
            draw_things_on_buffer(bitmap->pixels,lvl,draw_data);
          SDL_UnlockSurface(bitmap);
    }
}

int main (int argc, char *argv[])
{
  // Initialize the message displaying and storing
  init_messages();
  // Logging messages into file
  set_msglog_fname("viewmap.log");

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
  SDL_WM_SetCaption ("ADiKtEd Libray viewmap example", NULL);

  // create object for storing map
  level_init(&lvl,MFV_DKGOLD,NULL);
  // Set path to levels
  set_levels_path(lvl,"./Levels");
  // And to data files
  set_data_path(lvl,"./data");
  // Form a file name of the map to load.
  format_lvl_fname(lvl,"MAP00147");
//  format_lvl_fname(lvl,"MAP00011");

  // Sending message into buffer allows it to be logged
  // into file if we use set_msglog_fname() before.
  message_info("loading map file");

  // Loading map
  short result;
  result=user_load_map(lvl,0);
  if (result!=ERR_NONE)
  {
    // If an error occurs, then the message buffer
    // contains error message.
    // Release the error message.
    message_release();

    message_info("viewmap finished with map load error");

    level_free(lvl);
    level_deinit(&lvl);
    free_messages();
    return 3;
  }

  message_info("entering application loop");

  time_t begin_tm;
  time_t end_tm;
  // Prepare the time control
  time(&begin_tm);
  while (!done)
  {
      // Get input events
      process_events();
      // Draw to screen
      draw_screen();
      // Don't run too fast
      time(&end_tm);
      int delay=(end_tm-begin_tm);
      if (delay<ONE_LOOP_DELAY)
      {
        SDL_Delay(ONE_LOOP_DELAY-delay);
        // Increase amount of loops by one
        loop_count++;
      } else
      {
        // The loop has taken too much time - make bigger increase
        loop_count+=(delay/ONE_LOOP_DELAY);
      }
      begin_tm=end_tm;
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
