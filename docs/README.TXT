Adikted Dungeon Keeper Map Editor
------------------------------

Note - there is now a tutorial available for Adikted, at
http://www.yoda.arachsys.com/dk/tutorial.html. A text
version of this is also within this distribution, called
tutorial.txt. You may wish to print this out.

Here's the Win32 version executable. Just put all the
files in the same directory, edit map.ini as appropriate (don't
worry about changing datmode, that just changes the way round the
bytes on the .dat display are shown), and run it.
Usage: map [mapfile] [-m <logfile>] [-v] [-r] [-n] -s [mapfile] [-q]
where:
  mapfile  Specifies filename (eg newmap), or just map number
          (like 18 etc.) which is loaded. Skipping this will start
          with new map.
  -m logfile  Allows you to save all messages shown on the bottom
          of screen when Adkted is working. You must specify output
          file where the messages will be written (ie. adi_msg.log).
  -s [mapfile]  Saves the map. Can overwrite the opened file, or save
          to another one given as next parameter. Note that commands
          that come after this one are executed after the map is saved,
          and therefore are lost on quit.
  -v  Triggers map verification; you could combine it with "-m" option
          to get verification result in a text file.
  -r  Starts new, random map. If [mapfile] was specified, then
          it is loaded, but then cleared and replaced by the
          random map.
  -n  Forces to start new map, even if the [mapfile] was specified.
          The [mapfile] is loaded, but then replaced.
  -q  Quits Adikted after the tasks from previous parameters have been
          completed. Useful for automated work.
  -ds  Disable all sounds. Makes Adikted silent, and less annoying.
  -dvid  Disable showing anything on screen. Screen is not initied
         nor refreshed. Usefull if you want only run some commands
         and quit.
  -dinp  Disable keyboard input. Adikted will exit after executing
          all comands given in parameters. For users it works just
          like -q option, but it won't even init keyboard input.

When Adikted saves a map, it will ask you what you wish
to call it. I suggest you don't save directly over the Dungeon
Keeper original levels, but keep it in the current directory until
you're finished. When you're ready to test it, use a command like
the following to copy the files to a "real" location:

copy mymap.* c:\games\keeper\levels\map00001.*

You'll need a level script for your newly created level. You may be
able to get by with the script which comes with the original level 1
- ie just don't change anything - but if not, study the level
scripts for predefined levels and edit map00001.txt (or create a new
file newmap.txt, which will get picked up in the copy command above).

Press F1 for help.

Version: 0.42b
 Quit changed to ctrl+q
 Added load (ctrl+l) and new map (ctrl+n)
Version: 0.47b
 Fixed to allow compilation under new SLang
 Win32 version
 Fixed coord checking when placing a building
 Fixed keyboard codes to be more platform-independent
Version: 0.48b
 Added definitions of more objects, ie. creature lairs
 Added hero gate counting and numbering
 Some of code rewritten to be more clear
Version: 0.49b
 The way of interpreting user-typed map names is now unified
 All memory allocation routines checked and rewritten
 Fixed problem with automatic deletion of DHeart on saving
 Added some very beginnings of random map generator (ctrl+r)
Version: 0.49b
 The random map generator has been extended
 Added loading and saving of INF,TXT,LGT and WLB files
 Re-added updating of DAT and CLM (but not TNG) when saving
Version: 0.50b
 Most of the TNG support code rewritten completely
 Renewed "creature" screen (adding creatures)
 New item screen (allows adding ANY item available in the game)
 Create room effect function (e key in "thing" mode)
Version: 0.51b
 Enhanced map verification function (press v in slab mode)
 Completely rewritten CLM/DAT support (it was lot of work...)
Version: 0.51a
 New, dynamic CLM generation
 Completely new functions to geneate map graphic (DAT/CLM entries)
 Written support of all terrain types, but no rooms
 No automatic 'thing' generation
 Graffiti not working due to DAT support changes
 Project moved back to alpha state
Version: 0.52a
 Added support of Dungeon Heart, Portal, Library, Temple
 Some fixups and expansions in TNG support
 Added some command line parameters
 Rewritten screen refreshing, new look of Thing mode
 Help file format extended
 Clipboard can now copy action points and static lights
 More items can be switched to similar using s/x
Version: 0.53a
 Compass view under ctrl+p
 Added support of Hatchery, Lair, Graveyard, Barracks, Train room
 Better function for generating rooms surrounded by lava/water
Version: 0.54a
 Support of all rooms completed
 Graffiti support renewed
 Created ADI file format, its reading and writing
TODO before beta:
 Auto creating room things

Author:
Jon Skeet, skeet@pobox.com

Dev-C++ IDE version, fixations:
Tomasz Lis
