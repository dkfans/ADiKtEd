/******************************************************************************/
/** @file putgems.c
 * ADiKtEd library example 1.
 * @par Purpose:
 *     This example puts gems on center of gold vein in map 1.
 *     Demonstrates how to load/save map, and how to change slabs.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     27 Jul 2008 - 29 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "../libadikted/adikted.h"

int main(int argc, char *argv[])
{
  struct LEVEL *lvl;
  short result;

  // create object for storing map
  level_init(&lvl);

  // Initialize the message displaying and storing
  // (we won't use them in this example,
  // but still they have to be initialized)
  init_messages();

  printf("\nexample1: how to put gems on map\n\n");

  // Setting file name of the map to load
  format_lvl_fname(lvl,"Levels/MAP00001");

  printf("loading map file\n");

  // Loading map
  result=user_load_map(lvl,0);
  if (result!=ERR_NONE)
  {
    printf("cannot load map\n");
    printf("example1 finished with error\n");
    system("pause");	

    // The following two commands should be used to free memory
    // allocated for level
    level_free(lvl);
    level_deinit(&lvl);

    // This command should be always last function used from library
    free_messages();
    return 1;
  }

  // Making modifications
  printf("changing slab\n");
  // Putting gems
  int tile_x=47;
  int tile_y=43;
  unsigned short room=SLAB_TYPE_GEMS;
  user_set_slab(lvl,tile_x,tile_y,room);

  // Writing the map on same file name
  result=user_save_map(lvl,0);
  if (result!=ERR_NONE)
  {
    printf("cannot save map\n");
    printf("example1 finished with error\n");
    system("pause");	

    // The following two commands should be used to free memory
    // allocated for level
    level_free(lvl);
    level_deinit(&lvl);

    // This command should be always last function used from library
    free_messages();
    return 1;
  }
  printf("map \"%s\" saved\n", get_lvl_savfname(lvl));
  printf("example1 finished successfully\n");
  system("pause");	

  // The following two commands should be used to free memory
  // allocated for level
  level_free(lvl);
  level_deinit(&lvl);

  // This command should be always last function used from library
  free_messages();
  return 0;
}
