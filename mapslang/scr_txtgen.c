/******************************************************************************/
// scr_txtgen.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  27 Jan 2008

// Purpose:
//   Defines functions for part the script screen which handles automatic
//   generation menu. This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scr_txtgen.h"

#include "../libadikted/adikted.h"
#include "var_utils.h"
#include "input_kb.h"
#include "output_scr.h"
#include "textmenu.h"
#include "scr_actn.h"
#include "scr_txted.h"

void add_menu_player_labels(menu m,int first_x,int last_x,int y);

static char *AVAIL[4]={"None", "Rsch", "Got", NULL};
static unsigned int avail_indices[]={AVAIL_NO, AVAIL_RESEARCH, AVAIL_INSTANT,};

static unsigned int traps_indices[]={1, 2, 3, 4, 5, 6,};

static unsigned int doors_indices[]={1, 2, 3, 4,};

static unsigned int spells_indices[]={1, 17, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 18,};

// Note: order of this array must match adikted library
// (this is why the two first entries must exist)
static unsigned int rooms_indices[]={
    SLAB_TYPE_DUNGHEART, SLAB_TYPE_PORTAL, SLAB_TYPE_TREASURE,
    SLAB_TYPE_LIBRARY, SLAB_TYPE_PRISONCASE, SLAB_TYPE_TORTURE,
    SLAB_TYPE_TRAINING, SLAB_TYPE_WORKSHOP, SLAB_TYPE_SCAVENGER,
    SLAB_TYPE_TEMPLE, SLAB_TYPE_GRAVEYARD, SLAB_TYPE_HATCHERY,
    SLAB_TYPE_LAIR, SLAB_TYPE_BARRACKS, SLAB_TYPE_BRIDGE,
    SLAB_TYPE_GUARDPOST,
       };

static unsigned int monsters_indices[]={
    CREATR_SUBTP_IMP, CREATR_SUBTP_FLY, CREATR_SUBTP_BEETLE,
    CREATR_SUBTP_SPIDER, CREATR_SUBTP_BILEDEMN, CREATR_SUBTP_SPAWN,
    CREATR_SUBTP_ORC, CREATR_SUBTP_TROLL, CREATR_SUBTP_WARLOCK,
    CREATR_SUBTP_DRAGON, CREATR_SUBTP_MISTRESS, CREATR_SUBTP_HOUND,
    CREATR_SUBTP_HORNY, CREATR_SUBTP_VAMPIRE, CREATR_SUBTP_TENTCL,
    CREATR_SUBTP_SKELETON, CREATR_SUBTP_GHOST,
       };

static unsigned int heroes_indices[]={
    CREATR_SUBTP_TUNELER, CREATR_SUBTP_DWAFT, CREATR_SUBTP_THEFT,
    CREATR_SUBTP_SMURI, CREATR_SUBTP_ARCHER, CREATR_SUBTP_MONK,
    CREATR_SUBTP_BARBARIN, CREATR_SUBTP_WIZRD, CREATR_SUBTP_GIANT,
    CREATR_SUBTP_FAIRY, CREATR_SUBTP_WITCH, CREATR_SUBTP_KNIGHT,
    CREATR_SUBTP_AVATAR,CREATR_SUBTP_FLOAT,
       };

short scrptgen_process_special(struct WORKMODE_DATA *workdata,union _value val,void *param);

/*
 * Initializes variables for the script generation screen.
 */
short init_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    //Creating and clearing scrpt variable
    workdata->txtgen=(struct TXTGEN_DATA *)malloc(sizeof(struct TXTGEN_DATA));
    if (workdata->txtgen==NULL)
     die("init_scrptgen: Cannot allocate memory.");
    menu cmenu=init_scrptgen_menus(workdata);
    workdata->txtgen->currmnu=new_workmenu(cmenu);
    workdata->txtgen->currmnu->process_special=scrptgen_process_special;
    return true;
}

/*
 * Deallocates memory for the script generation screen.
 */
void free_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
  free(workdata->txtgen);
}

/*
 * Starts the script generator menu in script mode.
 */
short start_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if ((workdata==NULL)||(workdata->editor==NULL)) return false;
    if (!scrptgen_load_mnuparams(workdata->txtgen,workdata->lvl))
        return false;
    workdata->editor->gen_flags|=SGF_ENABLED;
    return true;
}

/*
 * Closes the script generator menu in script mode.
 */
void end_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if ((workdata==NULL)||(workdata->editor==NULL)) return;
    // TODO: ask for save changes
    scrptgen_store_mnuparams(workdata->lvl,workdata->txtgen);
    workdata->editor->gen_flags&=(~SGF_ENABLED);
}

/*
 * Loads the values from script into generator menu.
 */
short scrptgen_load_mnuparams(struct TXTGEN_DATA *tgen,struct LEVEL *lvl)
{
    struct DK_SCRIPT_PARAMETERS *par=level_get_script_param(lvl);
    if ((tgen==NULL)||(par==NULL)) return false;
    tgen->portal_genspd=par->portal_gen_speed;
    int i;
    int max_idx;
    for (i=0;i<PLAYERS_COUNT;i++)
    {
      tgen->plcrtrmax[i]=par->player[i].max_creatures;
      tgen->plgold[i]=par->player[i].start_gold;
      tgen->plcomput[i]=par->player[i].computer_player;
      // Double arrays with first indices PLAYER0..PLAYER_UNSET
      int k;
      for (k=0;k<PLAYERS_COUNT;k++)
      {
        tgen->pally[k][i]=par->player[i].ally[k];
      }
      max_idx=sizeof(monsters_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        tgen->mavail[k][i]=(par->player[i].creature_avail[monsters_indices[k]]==AVAIL_INSTANT);
      }
      max_idx=sizeof(heroes_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        tgen->havail[k][i]=(par->player[i].creature_avail[heroes_indices[k]]==AVAIL_INSTANT);
      }
      max_idx=sizeof(traps_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        tgen->tavail[k][i]=par->player[i].trap_avail[traps_indices[k]];
        tgen->tamount[k][i]=par->player[i].trap_amount[traps_indices[k]];
      }
      max_idx=sizeof(doors_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        tgen->davail[k][i]=par->player[i].door_avail[doors_indices[k]];
        tgen->damount[k][i]=par->player[i].door_amount[doors_indices[k]];
      }
      max_idx=sizeof(rooms_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        // Note: no rooms_indices[k] here
        tgen->ravail[k][i]=par->player[i].room_avail[k];
      }
      max_idx=sizeof(spells_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        tgen->savail[k][i]=par->player[i].spell_avail[spells_indices[k]];
      }
    }
    max_idx=sizeof(monsters_indices)/sizeof(unsigned int);
    for (i=0;i<max_idx;i++)
      tgen->mpool[i]=par->creature_pool[monsters_indices[i]];
    max_idx=sizeof(heroes_indices)/sizeof(unsigned int);
    for (i=0;i<max_idx;i++)
      tgen->hpool[i]=par->creature_pool[heroes_indices[i]];
    return true;
}

/*
 * Stores the changes made to script using the generator menu.
 */
short scrptgen_store_mnuparams(struct LEVEL *lvl,const struct TXTGEN_DATA *tgen)
{
    struct DK_SCRIPT_PARAMETERS *par=level_get_script_param(lvl);
    if ((tgen==NULL)||(par==NULL)) return false;
    par->portal_gen_speed=tgen->portal_genspd;
    int i;
    int max_idx;
    for (i=0;i<PLAYERS_COUNT;i++)
    {
      par->player[i].max_creatures=tgen->plcrtrmax[i];
      par->player[i].start_gold=tgen->plgold[i];
      par->player[i].computer_player=tgen->plcomput[i];
      // Double arrays with first indices PLAYER0..PLAYER_UNSET
      int k;
      for (k=0;k<PLAYERS_COUNT;k++)
      {
        par->player[i].ally[k]=tgen->pally[k][i];
      }
      max_idx=sizeof(monsters_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        short avail;
        if (tgen->mavail[k][i])
          avail=AVAIL_INSTANT;
        else
          avail=AVAIL_NO;
        par->player[i].creature_avail[monsters_indices[k]]=avail;
      }
      max_idx=sizeof(heroes_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        short avail;
        if (tgen->havail[k][i])
          avail=AVAIL_INSTANT;
        else
          avail=AVAIL_NO;
        par->player[i].creature_avail[heroes_indices[k]]=avail;
      }
      max_idx=sizeof(traps_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        par->player[i].trap_avail[traps_indices[k]]=tgen->tavail[k][i];
        par->player[i].trap_amount[traps_indices[k]]=tgen->tamount[k][i];
      }
      max_idx=sizeof(doors_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        par->player[i].door_avail[doors_indices[k]]=tgen->davail[k][i];
        par->player[i].door_amount[doors_indices[k]]=tgen->damount[k][i];
      }
      max_idx=sizeof(rooms_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        // Note: no rooms_indices[k] here
        par->player[i].room_avail[k]=tgen->ravail[k][i];
      }
      max_idx=sizeof(spells_indices)/sizeof(unsigned int);
      for (k=0;k<max_idx;k++)
      {
        par->player[i].spell_avail[spells_indices[k]]=tgen->savail[k][i];
      }
    }
    max_idx=sizeof(monsters_indices)/sizeof(unsigned int);
    for (i=0;i<max_idx;i++)
      par->creature_pool[monsters_indices[i]]=tgen->mpool[i];
    max_idx=sizeof(heroes_indices)/sizeof(unsigned int);
    for (i=0;i<max_idx;i++)
      par->creature_pool[heroes_indices[i]]=tgen->hpool[i];
    //TODO: write into script
    return true;
}

/*
 * Processes special menu commands in script generator.
 */
short scrptgen_process_special(struct WORKMODE_DATA *workdata,union _value val,void *param)
{
    int i;
    int j;
    int k;
    
    j = val.num%100;
    k=val.num/1000;
    // Grotty, but it'll do for now... shame I don't 
    // have actual long **s earlier
    struct TXTGEN_DATA *tgen=workdata->txtgen;
    switch ((val.num%1000)/100)
    {
    case 0:
      for (i=0; i < 5; i++)
        tgen->savail[j][i]=k;
      return true;
    case 1:
      for (i=0; i < 5; i++)
        tgen->ravail[j][i]=k;
      return true;
    case 2:
      for (i=0; i < 5; i++)
        tgen->tavail[j][i]=k;
      return true;
    case 3:
      for (i=0; i < 5; i++)
        tgen->davail[j][i]=k;
      return true;
    case 4:
      for (i=0; i < 5; i++)
        tgen->havail[j][i]=k;
      return true;
    case 5:
      for (i=0; i < 5; i++)
        tgen->mavail[j][i]=k;
      return true;
    }
    return false;
}

void add_menu_player_labels(menu m,int first_x,int last_x,int y)
{
    add_item_to_menu(m, menu_new_item (first_x, y, "Player 0", MIT_STATIC, menu_value_longptr(NULL), NULL));
    int width=last_x-first_x+1;
    add_item_to_menu(m, menu_new_item (first_x+((70*width)>>8), y, "1", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_item_to_menu(m, menu_new_item (first_x+((115*width)>>8), y, "2", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_item_to_menu(m, menu_new_item (first_x+((153*width)>>8), y, "3", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_item_to_menu(m, menu_new_item (first_x+((198*width)>>8), y, "Heroes", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_item_to_menu(m, menu_new_item (last_x, y, "(All players)", MIT_STATIC, menu_value_longptr(NULL), NULL));
}

struct _menu *init_scrptgen_menus(struct WORKMODE_DATA *workdata)
{
    int i, j;
    menu root, monst, hero, rooms, trapdoors, misc, spells;
    menuitem item;
    struct TXTGEN_DATA *tgen=workdata->txtgen;
    int max_idx;
    int sx;
    int lxs[5]={0, 10, 17, 24, 31};
    int lxm[5]={0, 10, 16, 22, 28};
    
    sx=17;
    spells = new_menu("Spell availability");
    add_item_to_menu(spells, menu_new_item (2, 2, "Spell", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_menu_player_labels(spells,sx+1,sx+41,2);
    max_idx=sizeof(spells_indices)/sizeof(unsigned int);
    for (i=0; i < max_idx; i++)
    {
      int adi_idx=spells_indices[i];
      add_item_to_menu(spells, menu_new_item (2, i+4, spell_cmd_fullname(adi_idx), MIT_STATIC, menu_value_longptr(NULL), NULL));
      for (j=0; j < 5; j++)
        add_item_to_menu(spells, menu_new_item (sx+lxs[j], i+4, "", MIT_CHOICE, menu_value_longptr(&tgen->savail[i][j]), AVAIL));
      add_item_to_menu(spells, menu_new_item(sx+41, i+4, AVAIL[0], MIT_SPECIAL,  menu_value_num(i), NULL));
      add_item_to_menu(spells, menu_new_item(sx+47, i+4, AVAIL[1], MIT_SPECIAL, menu_value_num(i+1000), NULL));
      add_item_to_menu(spells, menu_new_item(sx+52, i+4, AVAIL[2], MIT_SPECIAL, menu_value_num(i+2000), NULL));
    }

    sx=19;
    rooms = new_menu("Room availability");
    add_item_to_menu(rooms, menu_new_item (2, 2, "Room", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_menu_player_labels(rooms,sx+1,sx+41,2);
    max_idx=sizeof(rooms_indices)/sizeof(unsigned int)-2;
    for (i=0; i < max_idx; i++)
    {
      int adi_idx=rooms_indices[i+2];
      add_item_to_menu(rooms, menu_new_item (2, i+4, get_slab_fullname(adi_idx), MIT_STATIC, menu_value_longptr(NULL), NULL));
      for (j=0; j < 5; j++)
        add_item_to_menu(rooms, menu_new_item (sx+lxs[j], i+4, "", MIT_CHOICE, menu_value_longptr(&tgen->ravail[i+2][j]), AVAIL));
      add_item_to_menu(rooms, menu_new_item (sx+41, i+4, AVAIL[0], MIT_SPECIAL, menu_value_num(i+2+100), NULL));
      add_item_to_menu(rooms, menu_new_item (sx+47, i+4, AVAIL[1], MIT_SPECIAL, menu_value_num(i+2+1100), NULL));
      add_item_to_menu(rooms, menu_new_item (sx+52, i+4, AVAIL[2], MIT_SPECIAL, menu_value_num(i+2+2100), NULL));
    }
    
    sx=17;
    trapdoors = new_menu("Trap and door availability");
    add_item_to_menu(trapdoors, menu_new_item (2, 2, "Trap", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_menu_player_labels(trapdoors,sx+1,sx+41,2);
    max_idx=sizeof(traps_indices)/sizeof(unsigned int);
    for (i=0; i < max_idx; i++)
    {
      int adi_idx=traps_indices[i];
      add_item_to_menu(trapdoors, menu_new_item (2, i+4, trap_cmd_fullname(adi_idx), MIT_STATIC, menu_value_longptr(NULL), NULL));
      for (j=0; j < 5; j++)
        add_item_to_menu(trapdoors, menu_new_item (sx+lxs[j], i+4, "", MIT_CHOICE, menu_value_longptr(&tgen->tavail[i][j]), AVAIL));
      add_item_to_menu(trapdoors, menu_new_item (sx+41, i+4, AVAIL[0], MIT_SPECIAL, menu_value_num(i+100), NULL));
      add_item_to_menu(trapdoors, menu_new_item (sx+47, i+4, AVAIL[1], MIT_SPECIAL, menu_value_num(i+1200), NULL));
      add_item_to_menu(trapdoors, menu_new_item (sx+52, i+4, AVAIL[2], MIT_SPECIAL, menu_value_num(i+2200), NULL));
    }
    add_item_to_menu(trapdoors, menu_new_item (2, 11, "Door", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_menu_player_labels(trapdoors,sx+1,sx+41,11);
    max_idx=sizeof(doors_indices)/sizeof(unsigned int);
    for (i=0; i < max_idx; i++)
    {
      int adi_idx=doors_indices[i];
      add_item_to_menu(trapdoors, menu_new_item (2, i+13, door_cmd_fullname(adi_idx), MIT_STATIC, menu_value_longptr(NULL), NULL));
      for (j=0; j < 5; j++)
        add_item_to_menu(trapdoors, menu_new_item (sx+lxs[j], i+13, "", MIT_CHOICE, menu_value_longptr(&tgen->davail[i][j]), AVAIL));
      add_item_to_menu(trapdoors, menu_new_item (sx+41, i+13, AVAIL[0], MIT_SPECIAL, menu_value_num(i+300), NULL));
      add_item_to_menu(trapdoors, menu_new_item (sx+47, i+13, AVAIL[1], MIT_SPECIAL, menu_value_num(i+1300), NULL));
      add_item_to_menu(trapdoors, menu_new_item (sx+52, i+13, AVAIL[2], MIT_SPECIAL, menu_value_num(i+2300), NULL));
    }
    
    sx=24;
    hero = new_menu("Hero availability");
    add_menu_player_labels(hero,sx+1,sx+37,2);
    add_item_to_menu(trapdoors, menu_new_item (2, 2, "Hero", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_item_to_menu(hero, menu_new_item (19, 2, "Pool", MIT_STATIC, menu_value_longptr(NULL), NULL));
    max_idx=sizeof(heroes_indices)/sizeof(unsigned int);
    for (i=0; i < max_idx; i++)
    {
      int adi_idx=heroes_indices[i];
      add_item_to_menu(hero, menu_new_item (2, i+4, get_creature_subtype_fullname(adi_idx), MIT_STATIC, menu_value_longptr(NULL), NULL));
      item=menu_new_item(18, i+4, "", MIT_NUMERIC, menu_value_longptr(&tgen->hpool[i]), NULL);
      menu_item_limits(item,0,65535);
      add_item_to_menu(hero,item);
      for (j=0; j < 5; j++)
        add_item_to_menu(hero, menu_new_item (sx+lxm[j], i+4, "", MIT_BOOLEAN, menu_value_longptr(&tgen->havail[i][j]), NULL));
      add_item_to_menu(hero, menu_new_item (sx+38, i+4, "No", MIT_SPECIAL, menu_value_num(i+400), NULL));
      add_item_to_menu(hero, menu_new_item (sx+44, i+4, "Yes", MIT_SPECIAL, menu_value_num(i+1400), NULL));
    }

    sx=24;
    monst = new_menu("Monster availability");
    add_menu_player_labels(monst,sx+1,sx+37,2);
    add_item_to_menu(trapdoors, menu_new_item (2, 2, "Monster", MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_item_to_menu(monst, menu_new_item (19, 2, "Pool", MIT_STATIC, menu_value_longptr(NULL), NULL));
    max_idx=sizeof(monsters_indices)/sizeof(unsigned int);
    for (i=0; i < max_idx; i++)
    {
      int adi_idx=monsters_indices[i];
      add_item_to_menu(monst, menu_new_item (2, i+4, get_creature_subtype_fullname(adi_idx), MIT_STATIC, menu_value_longptr(NULL), NULL));
      item=menu_new_item(18, i+4, "", MIT_NUMERIC, menu_value_longptr(&tgen->mpool[i]), NULL);
      menu_item_limits(item,0,65535);
      add_item_to_menu(monst,item);
      for (j=0; j < 5; j++)
        add_item_to_menu(monst, menu_new_item (sx+lxm[j], i+4, "", MIT_BOOLEAN, menu_value_longptr(&tgen->mavail[i][j]), NULL));
      add_item_to_menu(monst, menu_new_item (sx+38, i+4, "No", MIT_SPECIAL, menu_value_num(i+500), NULL));
      add_item_to_menu(monst, menu_new_item (sx+44, i+4, "Yes", MIT_SPECIAL, menu_value_num(i+1500), NULL));
    }

    sx=24;
    misc = new_menu("Miscellaneous");
    add_item_to_menu(misc, menu_new_item (1, 1, "(empty)", MIT_STATIC, menu_value_longptr(NULL), NULL));
    
    // Create root menu
    
    root=new_menu(PROGRAM_NAME" Script Generator");
    root->back=NULL;
    add_item_to_menu(root, menu_new_item (2, 2, "Spells", MIT_MENU, menu_value_longptr(NULL), spells));
    spells->back=root;
    add_item_to_menu(root, menu_new_item (2, 3, "Creatures (monsters)", MIT_MENU, menu_value_longptr(NULL), monst));
    monst->back=root;
    add_item_to_menu(root, menu_new_item (2, 4, "Creatures (heroes)", MIT_MENU, menu_value_longptr(NULL), hero));
    hero->back=root;
    add_item_to_menu(root, menu_new_item (2, 5, "Rooms", MIT_MENU, menu_value_longptr(NULL), rooms));
    rooms->back=root;
    add_item_to_menu(root, menu_new_item (2, 6, "Traps and doors", MIT_MENU, menu_value_longptr(NULL), trapdoors));
    trapdoors->back=root;
    add_item_to_menu(root, menu_new_item (2, 7, "Miscellaneous", MIT_MENU, menu_value_longptr(NULL), misc));
    misc->back=root;

    add_item_to_menu(root, menu_new_item (2, 9, "On the availability menus, \"Rsch\" means you can research", 
                MIT_STATIC, menu_value_longptr(NULL), NULL));
    add_item_to_menu(root, menu_new_item (2, 10, "that item, \"Got\" means it is researched already.", 
                MIT_STATIC, menu_value_longptr(NULL), NULL));
         
    return root;
}
