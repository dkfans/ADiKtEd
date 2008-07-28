/******************************************************************************/
/** @file puttrain.c
 * ADiKtEd library example 2.
 * @par Purpose:
 *     This example puts 3x5 training room in bottom right part of map 1.
 *     Demonstrates how to change many slabs at once,
 *     and how to use the library's internal messages.
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

  // Initialize the message displaying and storing
  // We will use the messages for all text displayed
  // on screen by this example.
  init_messages();
  // Logging messages into file
  set_msglog_fname("puttrain.log");

  // Instead of printing the message directly, we will send it
  // to the internal messages system.
  message_info("example2: how to put a larger room on map");
  // Now we can easily get the message back, even multiple times.
  printf("%s\n",message_get());

  // create object for storing map
  level_init(&lvl);

  // Set paths to levels - this will allow us to
  // skip the path when formatting level name
  set_levels_path(lvl,"./Levels");

  // Form a file name of the map to load.
  // This time we will only give map number to
  // the function. Complete filename will be
  // formatted automatically.
  format_lvl_fname(lvl,"1");

  // As before, we're sending text message into internal buffer,
  // and getting it back from there. Sending message into buffer
  // allows it to be logged into file if we use set_msglog_fname()
  // before.
  message_info("loading map file");
  printf("%s\n",message_get());

  // Loading map
  result=user_load_map(lvl,0);
  if (result!=ERR_NONE)
  {
    // If an error occurs, then the message buffer
    // contains error message. We can show the message
    // like this:
    printf("%s\n",message_get());
    // If an error message is stored in buffer, lower priority
    // messages can't be written until we release the message.
    message_release();

    // Now, as the buffer is released, the message_info() function
    // can be used to store next message.
    message_info("example2 finished with load error");
    printf("%s\n",message_get());
    system("pause");	

    // The following two commands should be used to free memory
    // allocated for level
    level_free(lvl);
    level_deinit(&lvl);

    // This command should be always last function used from library
    free_messages();
    return 1;
  }

  message_info("map loaded successfully");
  printf("%s\n",message_get());
  // If an error message is stored in buffer, lower priority
  // messages can't be written until we release the message.
  message_release();

  // Making modifications

  message_info("placing room on map");
  printf("%s\n",message_get());

  // Putting unowed training room
  unsigned short room=SLAB_TYPE_TRAINING;
  unsigned short owner=PLAYER_UNSET;
  user_set_slabown_rect(lvl,47,49,53,57,room,owner);

  // Writing the map on same file name
  result=user_save_map(lvl,0);
  if (result!=ERR_NONE)
  {
    // If an error occurs, then the message buffer
    // contains error message. We can show the message
    // like this:
    printf("%s\n",message_get());
    // If an error message is stored in buffer, lower priority
    // messages can't be written until we release the message.
    message_release();

    // Now, as the buffer is released, the message_info() function
    // can be used to store next message.
    message_info("example2 finished with save error");
    printf("%s\n",message_get());
    system("pause");	

    // The following two commands should be used to free memory
    // allocated for level
    level_free(lvl);
    level_deinit(&lvl);

    // This command should be always last function used from library
    free_messages();
    return 1;
  }

  // The message_info() function can be used to make formatted
  // message, in the same way as printf.
  message_info("map \"%s\" saved", get_lvl_savfname(lvl));
  printf("%s\n",message_get());
  message_release();

  message_info("example2 finished successfully");
  printf("%s\n",message_get());
  system("pause");	

  // The following two commands should be used to free memory
  // allocated for level
  level_free(lvl);
  level_deinit(&lvl);

  // This command should be always last function used from library
  free_messages();
  return 0;
}
