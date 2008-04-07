Adikted Dungeon Keeper Map Editor
------------------------------

Note - there is now a tutorial available for Adikted, at
http://www.yoda.arachsys.com/dk/tutorial.html.
A version of this is also within this distribution,
called adikted_tutorial.html. You may wish to print this out.

Here's the Win32 version executable. Just put all the
files in the same directory, edit map.ini as appropriate (you should
change paths to your DK files; you can also change other parameters
if you want), and run it.
Usage: map [mapfile] [-m <logfile>] [-v] [-r] [-n] [-s [mapfile]] [-q]
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
  -du  Disable auto updates of both DAT/CLM/W?B and TNG/LGT/APT.
          With this, generating maps and starting new one will be very
          fast, but the update must be triggered manually before
          saving a map.
  -bmp  Write bitmap miniature of the map.
  -dvid  Disable showing anything on screen. Screen is not initied
         nor refreshed. Useful if you want only run some commands
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

TODO before final:
 Fixations in room things parameters (height,other)
 Fixations in room corner graphics
Version: 0.80b
 Unrecognized script commands converted to REMarks on update
 Added SCRIPT_LEVEL_SPACES command in INI file
 Added verification of script commands with amount of arguments
 Action point number can be changed using s/x
 Fixed hero gate numbering (positive number instead of negative)
 Positions and ranges can be displayed as fixed point numbers
 Most of the script command parameters are verified
 Action point/hero grate numbers verified in script file
 Added colors and highlighting in script mode
 Little corrections to save/load screens
Version: 0.75b
 Script file recognition logic extended
 Modified frail columns generation to prevent stucked imps
 Static light intensivity can now be changed using s/x
 Added basic verification of script commands (only command names)
 Added experimental script update function ('u' in script mode)
 Fixed sensitive tile computing for various things
 Extended items verification
 Traps, doors and creatures are now numbered (as in official editor)
Version: 0.74b
 Extended CLM entries verification
 Texture animations are readed when creating BMPs
 Correction in BMP resizing algorithm
 File reading routine rewritten, better error handling
 Added help about elements of various lists
 Fixed some mistakes in TNG logic, rewritten TNG categorization
 Added "edit object" function (enter key in Thing mode)
Version: 0.73b
 Added space saving if screen size is small
 INI file commented, and added some options
 End of text line is now written as "\r\n", not only "\n"
 Level graphic generation improved and more adjustable
 Fixed typing path in load/save screens
 Changed the way of clearing unused CLM entries
 CLM generation code divided to more files
 A little more cubes defined
 Small correction to BMP creation function
Version: 0.72b
 Unified initialization of work modes (probably some errors fixed)
 New Load Map (ctrl+l) and Save screens (with preview)
 Fixed hanging on ctrl+g key
 Extended logging ("-m" command line option)
 Some corrections to "add creature" screen
 Some new "custom columns"
 New graffiti entering screen ('d' in slab mode)
Version: 0.71b
 Created some basic structs to close variables
 When verifying level ('v') errors are now highlighted
 Default effects range set to 5 subtiles
 Some global variables replaced by locals
 Fixed setting solid property for custom columns
Version: 0.70b
 Map miniature size can be now changed in INI file
 Fixed damaging the script file
 Corrected height of Guard flag
Version: 0.69b
 Added bitmap miniature making (ctrl+b)
Version: 0.68b
 Script handling changed
 More script commands are recognized (but still not all)
 Heart flames are placed at proper height
Version: 0.67b
 More cube values are defined (invisible for end users)
 Some corrections in Column mode
 Custom columns function completely rewritten (speed optimized)
 Test map generation function, for recognizing cubes
 Map is loaded anyway if script loading fails
 Information after updating TNG is more detailed
Version: 0.66b
 Range of items which have such property is now visible in thing mode
 Option to disable objects range visibility in INI file
 INI file commands updated
 Fixed compile errors and makefile for Linux systems
 Added "-du" command line option
Version: 0.63b
 Added changing range of lights/effects with '-' and '+'
 The last column (which has no slab attached) is updated as rock
 Some of the load/save functions rewritten and fixed
 WLB files are now maintained and updated properly
 Loading/saving of FLG files
 FLG entries are maintained quite good
 Added room-specific reinforced walls
 Added searching for objects (ctrl+f)
 Added rework mode (undocumented - for reworking only)
Version: 0.60b
 Some corrections in screen colors
 Better statistics counting and numbering of hero gates
 Auto creating room things for all rooms
 Auto creating dungeon heart thing (one per player)
 Auto creating torches
 Updating owner now results in TNG update
 The '[' and ']' keys now works on action points
Version: 0.56a
 Some corrections in "manual columns"
 Texture change function under ctrl+e
 Enter key displays list of slabs
 Verification ('v') of things extended
 Items and room effects have now "sensitive tile"
 Added computing "sensitive tile" for torches and room effects
 Fixed some display problems if Adikted window is small
Version: 0.55a
 Fixed hanging when F7 (quick load) pressed
 Graffiti orientation can now be changed with 'a'
 Graffiti can now be oriented for view from top
 Extended help system and help file
 Corrected white spaces treatment inside map.ini file
 Height of graffiti/thing can be changed with '[' and ']'
 New, experimental "manual columns" function
Version: 0.54a
 Support of all rooms completed
 Graffiti support renewed
 Created ADI file format, its reading and writing
Version: 0.53a
 Compass view under ctrl+p
 Added support of Hatchery, Lair, Graveyard, Barracks, Train room
 Better function for generating rooms surrounded by lava/water
Version: 0.52a
 Added support of Dungeon Heart, Portal, Library, Temple
 Some fixups and expansions in TNG support
 Added some command line parameters
 Rewritten screen refreshing, new look of Thing mode
 Help file format extended
 Clipboard can now copy action points and static lights
 More items can be switched to similar using s/x
Version: 0.51a
 New, dynamic CLM generation
 Completely new functions to geneate map graphic (DAT/CLM entries)
 Written support of all terrain types, but no rooms
 No automatic 'thing' generation
 Graffiti not working due to DAT support changes
 Project moved back to alpha state
Version: 0.51b
 Enhanced map verification function (press v in slab mode)
 Completely rewritten CLM/DAT support (it was lot of work...)
Version: 0.50b
 Most of the TNG support code rewritten completely
 Renewed "creature" screen (adding creatures)
 New item screen (allows adding ANY item available in the game)
 Create room effect function (e key in "thing" mode)
Version: 0.49b
 The random map generator has been extended
 Added loading and saving of INF,TXT,LGT and WLB files
 Re-added updating of DAT and CLM (but not TNG) when saving
Version: 0.48b
 The way of interpreting user-typed map names is now unified
 All memory allocation routines checked and rewritten
 Fixed problem with automatic deletion of DHeart on saving
 Added some very beginnings of random map generator (ctrl+r)
Version: 0.47b
 Added definitions of more objects, ie. creature lairs
 Added hero gate counting and numbering
 Some of code rewritten to be more clear
Version: 0.46b
 Fixed to allow compilation under new SLang
 Win32 version
 Fixed coord checking when placing a building
 Fixed keyboard codes to be more platform-independent
Version: 0.42b
 Quit changed to ctrl+q
 Added load (ctrl+l) and new map (ctrl+n)

Author:
 Jon Skeet, skeet@pobox.com

Dev-C++ IDE version,
rewritten most of the code:
 Tomasz Lis
