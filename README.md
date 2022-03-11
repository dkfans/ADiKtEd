# Adikted Dungeon Keeper Map Editor

Note - there is now a manual available for ADiKtEd.
It even includes a basic tutorial to quickly learn the program.
A version of it is included within this distribution,
called `dk_adikted_manual.htm`. You may wish to print this out.
## Build

### Unix-like systems

#### CMake

Run 
`cmake --install-prefix=/usr . && make install`
and copy the examples to the keeperfx directory

### Windows

#### make

Run `make -f Makefile.win`

Here's the Win32 version executable. Just put all the
files in the same directory, edit map.ini as appropriate (you should
change paths to your DK files; you can also change other parameters
if you want), and run it.

## Usage

Run `map [mapfile] [-m <logfile>] [-v] [-r] [-n] [-s [mapfile]] [-q]`

When ADiKtEd saves a map, it will ask you what you wish to call it
(unless you're not using quick save). I suggest you don't save
directly over the Dungeon Keeper original levels, but keep it
in the current directory until you're finished.
Then, at end, save it on `map00001` to access it easily in the game.

You'll need a level script for your newly created level. You may be
able to get by with the script which comes with the original level 1
- ie just copy it and paste into TXT file of your new map - but
if not, study the level scripts reference from `dk_scripting_ref.htm`.
You can also try looking at the original DK and DD levels for examples.

Press F1 for help.

## TODO before final
 Fixations in room things parameters (height,other)
 Fixations in room corner graphics

## Author
 Jon Skeet, skeet@pobox.com

Dev-C++ IDE version,
rewritten most of the code:
 Tomasz Lis
