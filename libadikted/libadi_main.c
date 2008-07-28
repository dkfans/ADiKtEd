
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

/** \mainpage
 *
 * \section intro_sec Introduction
 *
 * The libAdikted dynamic library contains all you need to load, edit and write
 * Dungeon Keeper 1 maps. Written in Ansi-C, it provides complete API for
 * maintaining the map files and modifying the map in every way you can imagine.
 *
 * \section install_sec Installation
 *
 * To install libAdikted, just copy it into your sources folder and use 'make'.
 * After that, you're ready to use the library in your projects.
 *
 * Under Windows OSes you don't even need to recompile the library - just use it,
 * including some of the header files and calling the functions.
 *
 * \section usage_sec Usage
 *
 * To learn how to use libAdikted, look at the examples provided. Also, look at
 * the ADiKtEd executable sources.
 *
 * \example putgems.c
 *     This example puts gems on center of gold vein in map 1.
 * \example puttrain.c
 *     This example puts 3x5 training room in bottom right part of map 1.
 */

BOOL APIENTRY DllMain (HINSTANCE hInst,    // Library instance handle.
                       DWORD reason,       // Reason this function is being called.
                       LPVOID reserved )   // Not used.
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
        break;

      case DLL_PROCESS_DETACH:
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    // Returns TRUE on success, FALSE on failure
    return TRUE;
}
