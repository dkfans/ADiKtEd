/*
 * lev_data.c
 *
 * Defines functions for maintaining the level meory structure.
 *
 */

#include "lev_data.h"

#include "globals.h"

LEVEL *lvl=NULL;

/*
 * creates object for storing one level
 */
short level_init()
{
  lvl=(LEVEL *)malloc(sizeof(LEVEL));
  if (lvl==NULL)
      die("level_init: Out of memory");
  lvl->slb=NULL;
  lvl->own=NULL;
  lvl->wib=NULL;
  lvl->clm=NULL;
  lvl->tng_lookup=NULL;
  lvl->tng_nums=NULL;
  lvl->tng_subnums=NULL;
  lvl->stats.hero_gates_count=0;
}
