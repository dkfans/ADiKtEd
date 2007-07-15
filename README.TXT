Adikted Dungeon Keeper Map Editor
------------------------------

Note - there is now a tutorial available for Adikted, at
http://www.yoda.arachsys.com/dk/tutorial.html. A text
version of this is also within this distribution, called
tutorial.txt. You may wish to print this out.

Here's the Win32 version executable. Just put all the
files in the same directory, edit map.ini as appropriate (don't
worry about changing datmode, that just changes the way round the
bytes on the .dat display are shown), and run it. Running it with a
level number tries to find that level using the filebase directive
in map.ini. Running it with a filename (eg newmap) tries to load
that map from the current directory. Running it with no options
starts a new map. When it saves a map, it will ask you what you wish
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

Author:
Jon Skeet, skeet@pobox.com

Dev-C++ IDE version, fixations:
Tomasz Lis
