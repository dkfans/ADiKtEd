/******************************************************************************/
/** @file obj_things.h
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Header file. Defines exported routines from obj_things.c
 * @par Comment:
 *     None.
 * @author   Jon Skeet, Tomasz Lis
 * @date     14 Oct 1997
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef ADIKT_OBJTHINGS_H
#define ADIKT_OBJTHINGS_H

#include "globals.h"

/*Things - most of the game objects is stored as thing */
#define THING_TYPE_NONE        0x00
#define THING_TYPE_ITEM        0x01 /* called "object" in the game */
#define THING_TYPE_SHOT        0x02
#define THING_TYPE_EFFCTELEM   0x03 /* "effect element" (?) */
#define THING_TYPE_DEADCRTR    0x04 /* dead creature */
#define THING_TYPE_CREATURE    0x05
#define THING_TYPE_EFFECTGEN   0x07 /* called "effect generator" in the game */
#define THING_TYPE_TRAP        0x08
#define THING_TYPE_DOOR        0x09
#define THING_TYPE_AMBNITSND   0x0c /* ambient sound */
#define THING_TYPE_CAVEIN      0x0d

/* Count of existing subtypes (max index+1) */
#define ITEM_STYPES_COUNT      0x87
#define CREATR_STYPES_COUNT    0x22
#define TRAP_STYPES_COUNT      0x0d
#define DOOR_STYPES_COUNT      0x05

/* sub-types for items */
#define ITEM_SUBTYPE_NULL      0x00
#define ITEM_SUBTYPE_BARREL    0x01
#define ITEM_SUBTYPE_TORCH     0x02
#define ITEM_SUBTYPE_GOLDCHEST 0x03
#define ITEM_SUBTYPE_TEMPLESTA 0x04
#define ITEM_SUBTYPE_DNHEART   0x05
#define ITEM_SUBTYPE_GOLD      0x06
#define ITEM_SUBTYPE_TORCHUN   0x07
#define ITEM_SUBTYPE_STATUEWO  0x08
#define ITEM_SUBTYPE_CHICKNGRW 0x09
#define ITEM_SUBTYPE_CHICKN    0x0a
#define ITEM_SUBTYPE_SPELLHOE  0x0b
#define ITEM_SUBTYPE_SPELLIMP  0x0c
#define ITEM_SUBTYPE_SPELLMUST 0x0d
#define ITEM_SUBTYPE_SPELLSLAP 0x0e
#define ITEM_SUBTYPE_SPELLSOE  0x0f
#define ITEM_SUBTYPE_SPELLCTA  0x10
#define ITEM_SUBTYPE_SPELLCAVI 0x11
#define ITEM_SUBTYPE_SPELLHEAL 0x12
#define ITEM_SUBTYPE_SPELLHLDA 0x13
#define ITEM_SUBTYPE_SPELLLIGH 0x14
#define ITEM_SUBTYPE_SPELLSPDC 0x15
#define ITEM_SUBTYPE_SPELLPROT 0x16
#define ITEM_SUBTYPE_SPELLCONC 0x17

#define ITEM_SUBTYPE_NULL1     0x18
#define ITEM_SUBTYPE_NULL2     0x19

#define ITEM_SUBTYPE_ANVIL     0x1a
#define ITEM_SUBTYPE_PRISONBAR 0x1b
#define ITEM_SUBTYPE_CANDLSTCK 0x1c /*candle with candlestick */
#define ITEM_SUBTYPE_GRAVSTONE 0x1d
#define ITEM_SUBTYPE_STATUHORN 0x1e
#define ITEM_SUBTYPE_TRAINPOST 0x1f
#define ITEM_SUBTYPE_TORTSPIKE 0x20
#define ITEM_SUBTYPE_TEMPLESPN 0x21 /*Temple Spangle */

#define ITEM_SUBTYPE_POTION1   0x22
#define ITEM_SUBTYPE_POTION2   0x23
#define ITEM_SUBTYPE_POTION3   0x24
#define ITEM_SUBTYPE_PWHAND    0x25
#define ITEM_SUBTYPE_PWHANDGRB 0x26
#define ITEM_SUBTYPE_PWHANDWHP 0x27
#define ITEM_SUBTYPE_CHICKNSTB 0x28
#define ITEM_SUBTYPE_CHICKNWOB 0x29
#define ITEM_SUBTYPE_CHICKNCRK 0x2a
#define ITEM_SUBTYPE_GOLDL     0x2b
#define ITEM_SUBTYPE_SPINNKEY  0x2c
#define ITEM_SUBTYPE_SPELLDISE 0x2d
#define ITEM_SUBTYPE_SPELLCHKN 0x2e
#define ITEM_SUBTYPE_SPELLDWAL 0x2f
#define ITEM_SUBTYPE_SPELLTBMB 0x30
#define ITEM_SUBTYPE_HEROGATE  0x31
#define ITEM_SUBTYPE_SPINNKEY2 0x32
        /*UNTESTED - indices may differ by one */
#define ITEM_SUBTYPE_ARMOUR    0x33
#define ITEM_SUBTYPE_GLDHOARD1 0x34
#define ITEM_SUBTYPE_GLDHOARD2 0x35
#define ITEM_SUBTYPE_GLDHOARD3 0x36
#define ITEM_SUBTYPE_GLDHOARD4 0x37
#define ITEM_SUBTYPE_GLDHOARD5 0x38
        /*Creature lairs */
#define ITEM_SUBTYPE_LAIRWIZRD 0x39
#define ITEM_SUBTYPE_LAIRBARBR 0x3a
#define ITEM_SUBTYPE_LAIRARCHR 0x3b
#define ITEM_SUBTYPE_LAIRMONK  0x3c
#define ITEM_SUBTYPE_LAIRDWRFA 0x3d
#define ITEM_SUBTYPE_LAIRKNGHT 0x3e
#define ITEM_SUBTYPE_LAIRAVATR 0x3f
#define ITEM_SUBTYPE_LAIRTUNLR 0x40
#define ITEM_SUBTYPE_LAIRWITCH 0x41
#define ITEM_SUBTYPE_LAIRGIANT 0x42
#define ITEM_SUBTYPE_LAIRFAIRY 0x43
#define ITEM_SUBTYPE_LAIRTHEFT 0x44
#define ITEM_SUBTYPE_LAIRSAMUR 0x45
#define ITEM_SUBTYPE_LAIRHORNY 0x46
#define ITEM_SUBTYPE_LAIRSKELT 0x47
#define ITEM_SUBTYPE_LAIRGOBLN 0x48
#define ITEM_SUBTYPE_LAIRDRAGN 0x49
#define ITEM_SUBTYPE_LAIRDEMSP 0x4a
#define ITEM_SUBTYPE_LAIRFLY   0x4b
#define ITEM_SUBTYPE_LAIRDKMIS 0x4c
#define ITEM_SUBTYPE_LAIRSORCR 0x4d
#define ITEM_SUBTYPE_LAIRBILDM 0x4e
#define ITEM_SUBTYPE_LAIRIMP   0x4f
#define ITEM_SUBTYPE_LAIRBUG   0x50
#define ITEM_SUBTYPE_LAIRVAMP  0x51
#define ITEM_SUBTYPE_LAIRSPIDR 0x52
#define ITEM_SUBTYPE_LAIRHLHND 0x53
#define ITEM_SUBTYPE_LAIRGHOST 0x54
#define ITEM_SUBTYPE_LAIRTENTC 0x55

#define ITEM_SUBTYPE_SPREVMAP  0x56
#define ITEM_SUBTYPE_SPRESURCT 0x57
#define ITEM_SUBTYPE_SPTRANSFR 0x58
#define ITEM_SUBTYPE_SPSTEALHR 0x59
#define ITEM_SUBTYPE_SPMULTPLY 0x5a
#define ITEM_SUBTYPE_SPINCLEV  0x5b
#define ITEM_SUBTYPE_SPMKSAFE  0x5c
#define ITEM_SUBTYPE_SPHIDNWRL 0x5d
#define ITEM_SUBTYPE_TBBOULDER 0x5e
#define ITEM_SUBTYPE_TBALARM   0x5f
#define ITEM_SUBTYPE_TBPOISONG 0x60
#define ITEM_SUBTYPE_TBLIGHTNG 0x61
#define ITEM_SUBTYPE_TBWRDOFPW 0x62
#define ITEM_SUBTYPE_TBLAVA    0x63
#define ITEM_SUBTYPE_TBDUMMY2  0x64
#define ITEM_SUBTYPE_TBDUMMY3  0x65
#define ITEM_SUBTYPE_TBDUMMY4  0x66
#define ITEM_SUBTYPE_TBDUMMY5  0x67
#define ITEM_SUBTYPE_TBDUMMY6  0x68
#define ITEM_SUBTYPE_TBDUMMY7  0x69
#define ITEM_SUBTYPE_DBWOOD    0x6a
#define ITEM_SUBTYPE_DBBRACE   0x6b
#define ITEM_SUBTYPE_DBSTEEL   0x6c
#define ITEM_SUBTYPE_DBMAGIC   0x6d
#define ITEM_SUBTYPE_WBITEM    0x6e
#define ITEM_SUBTYPE_HEARTFLMR 0x6f

#define ITEM_SUBTYPE_DISEASE   0x70
#define ITEM_SUBTYPE_SCAVNGEYE 0x71
#define ITEM_SUBTYPE_WRKSHPMCH 0x72
#define ITEM_SUBTYPE_GURDFLAGR 0x73
#define ITEM_SUBTYPE_GURDFLAGB 0x74
#define ITEM_SUBTYPE_GURDFLAGG 0x75
#define ITEM_SUBTYPE_GURDFLAGY 0x76
#define ITEM_SUBTYPE_FLAGPOST  0x77
#define ITEM_SUBTYPE_HEARTFLMB 0x78
#define ITEM_SUBTYPE_HEARTFLMG 0x79
#define ITEM_SUBTYPE_HEARTFLMY 0x7a
#define ITEM_SUBTYPE_PWSIGHT   0x7b
#define ITEM_SUBTYPE_PWLIGHTNG 0x7c
#define ITEM_SUBTYPE_TORTURER  0x7d
#define ITEM_SUBTYPE_LAIRORC   0x7e
#define ITEM_SUBTYPE_PWHANDGLD 0x7f
#define ITEM_SUBTYPE_SPINNCOIN 0x80
#define ITEM_SUBTYPE_STATUE2   0x81
#define ITEM_SUBTYPE_STATUE3   0x82
#define ITEM_SUBTYPE_STATUE4   0x83
#define ITEM_SUBTYPE_STATUE5   0x84
#define ITEM_SUBTYPE_SPCUSTOM  0x85
#define ITEM_SUBTYPE_SPELLARMG 0x86

/*Effects (all listed) */
#define EFCTGEN_SUBTP_LAVA     0x01
#define EFCTGEN_SUBTP_DRIPWTR  0x02
#define EFCTGEN_SUBTP_ROCKFAL  0x03
#define EFCTGEN_SUBTP_ENTRICE  0x04
#define EFCTGEN_SUBTP_DRYICE   0x05

/*Creature types (all listed) */
#define CREATR_SUBTP_WIZRD     0x01
#define CREATR_SUBTP_BARBARIN  0x02
#define CREATR_SUBTP_ARCHER    0x03
#define CREATR_SUBTP_MONK      0x04
#define CREATR_SUBTP_DWAFT     0x05
#define CREATR_SUBTP_KNIGHT    0x06
#define CREATR_SUBTP_AVATAR    0x07
#define CREATR_SUBTP_TUNELER   0x08
#define CREATR_SUBTP_WITCH     0x09
#define CREATR_SUBTP_GIANT     0x0A
#define CREATR_SUBTP_FAIRY     0x0B
#define CREATR_SUBTP_THEFT     0x0C
#define CREATR_SUBTP_SMURI     0x0D
#define CREATR_SUBTP_HORNY     0x0E
#define CREATR_SUBTP_SKELETON  0x0F
#define CREATR_SUBTP_TROLL     0x10
#define CREATR_SUBTP_DRAGON    0x11
#define CREATR_SUBTP_SPAWN     0x12
#define CREATR_SUBTP_FLY       0x13
#define CREATR_SUBTP_MISTRESS  0x14
#define CREATR_SUBTP_WARLOCK   0x15
#define CREATR_SUBTP_BILEDEMN  0x16
#define CREATR_SUBTP_IMP       0x17
#define CREATR_SUBTP_BEETLE    0x18
#define CREATR_SUBTP_VAMPIRE   0x19
#define CREATR_SUBTP_SPIDER    0x1A
#define CREATR_SUBTP_HOUND     0x1B
#define CREATR_SUBTP_GHOST     0x1C
#define CREATR_SUBTP_TENTCL    0x1D
#define CREATR_SUBTP_ORC       0x1E
#define CREATR_SUBTP_FLOAT     0x1F
/* Untested; shouldn't be used */
#define CREATR_SUBTP_NEWA      0x20
#define CREATR_SUBTP_NEWB      0x21

#define DOOR_SUBTYPE_WOOD      0x01
#define DOOR_SUBTYPE_BRACED    0x02
#define DOOR_SUBTYPE_IRON      0x03
#define DOOR_SUBTYPE_MAGIC     0x04

/*Traps (all listed; dummy doesn't really exist) */
#define TRAP_SUBTYPE_BOULDER   0x01
#define TRAP_SUBTYPE_ALARM     0x02
#define TRAP_SUBTYPE_GAS       0x03
#define TRAP_SUBTYPE_LIGHTNG   0x04
#define TRAP_SUBTYPE_WORDPWR   0x05
#define TRAP_SUBTYPE_LAVA      0x06
#define TRAP_SUBTYPE_DUMMY2    0x07
#define TRAP_SUBTYPE_DUMMY3    0x08
#define TRAP_SUBTYPE_DUMMY4    0x09
#define TRAP_SUBTYPE_DUMMY5    0x0A
#define TRAP_SUBTYPE_DUMMY6    0x0B
#define TRAP_SUBTYPE_DUMMY7    0x0C

#define DOOR_ORIENT_NSPASS     0x00
#define DOOR_ORIENT_EWPASS     0x01
#define DOOR_PASS_UNLOCKED     0x00
#define DOOR_PASS_LOCKED       0x01

/*Thing categories - indices must match the arrays in .c file */
/* the UNKNOWN category must be the last one, and THING_CATEGR_COUNT */
/* must be set to the count of categories */
#define THING_CATEGR_NULL       0x00
#define THING_CATEGR_EFFCTGEN   0x01
#define THING_CATEGR_ITEMEFFCT  0x02
#define THING_CATEGR_CREATR     0x03
#define THING_CATEGR_CREATLAIR  0x04
#define THING_CATEGR_TRAP       0x05
#define THING_CATEGR_DOOR       0x06
#define THING_CATEGR_SPECIALBOX 0x07
#define THING_CATEGR_SPELLBOOK  0x08
#define THING_CATEGR_WRKSHOPBOX 0x09
#define THING_CATEGR_SPINNTNG   0x0a
#define THING_CATEGR_FOOD       0x0b
#define THING_CATEGR_GOLD       0x0c
#define THING_CATEGR_TORCHCNDL  0x0d
#define THING_CATEGR_HEARTFLAME 0x0e
#define THING_CATEGR_POLEBAR    0x0f
#define THING_CATEGR_STATUE     0x10
#define THING_CATEGR_FURNITURE  0x11
#define THING_CATEGR_ROOMEQUIP  0x12
#define THING_CATEGR_PWHAND     0x13
#define THING_CATEGR_DNCRUCIAL  0x14
#define THING_CATEGR_UNKNOWN    0x15

/*Other defines */
#define THING_SENSITILE_NONE   0x0ffff

struct LEVEL;
struct IPOINT_2D;

DLLIMPORT unsigned char get_thing_type(const unsigned char *thing);
DLLIMPORT short set_thing_type(unsigned char *thing,unsigned char type_idx);
DLLIMPORT unsigned char get_thing_subtype(const unsigned char *thing);
DLLIMPORT short set_thing_subtype(unsigned char *thing,const unsigned char stype_idx);
DLLIMPORT unsigned char get_thing_owner(const unsigned char *thing);
DLLIMPORT short set_thing_owner(unsigned char *thing,unsigned char plyr_idx);

DLLIMPORT unsigned char get_thing_subtile_x(const unsigned char *thing);
DLLIMPORT unsigned short get_thing_pos_x_adv(const unsigned char *thing);
DLLIMPORT short set_thing_subtile_x(unsigned char *thing,unsigned char pos_x);
DLLIMPORT unsigned char get_thing_subtile_y(const unsigned char *thing);
DLLIMPORT unsigned short get_thing_pos_y_adv(const unsigned char *thing);
DLLIMPORT short set_thing_subtile_y(unsigned char *thing,unsigned char pos_y);
DLLIMPORT unsigned char get_thing_subtile_h(const unsigned char *thing);
DLLIMPORT short set_thing_subtile_h(unsigned char *thing,unsigned char pos_h);
DLLIMPORT short set_thing_subtile(unsigned char *thing,unsigned char pos_x,unsigned char pos_y);
DLLIMPORT unsigned char get_thing_subtpos_x(const unsigned char *thing);
DLLIMPORT short set_thing_subtpos_x(unsigned char *thing,unsigned char pos_x);
DLLIMPORT unsigned char get_thing_subtpos_y(const unsigned char *thing);
DLLIMPORT short set_thing_subtpos_y(unsigned char *thing,unsigned char pos_y);
DLLIMPORT unsigned char get_thing_subtpos_h(const unsigned char *thing);
DLLIMPORT short set_thing_subtpos_h(unsigned char *thing,unsigned char pos_h);
DLLIMPORT short set_thing_subtpos(unsigned char *thing,unsigned char pos_x,unsigned char pos_y);
DLLIMPORT unsigned char get_thing_level(const unsigned char *thing);
DLLIMPORT short set_thing_level(unsigned char *thing,unsigned char lev_num);
DLLIMPORT unsigned short get_thing_sensitile(const unsigned char *thing);
DLLIMPORT short set_thing_sensitile(unsigned char *thing,unsigned short til_num);

DLLIMPORT unsigned char get_door_orientation(const unsigned char *thing);
DLLIMPORT short set_door_orientation(unsigned char *thing,unsigned char orient);
DLLIMPORT unsigned char get_thing_range_subtpos(const unsigned char *thing);
DLLIMPORT short set_thing_range_subtpos(unsigned char *thing,unsigned char rng);
DLLIMPORT unsigned char get_thing_range_subtile(const unsigned char *thing);
DLLIMPORT short set_thing_range_subtile(unsigned char *thing,unsigned char rng);
DLLIMPORT unsigned int get_thing_range_adv(const unsigned char *thing);

DLLIMPORT unsigned char *create_thing_empty(void);
DLLIMPORT unsigned char *create_thing(unsigned int sx, unsigned int sy);
DLLIMPORT unsigned char *create_thing_copy(const struct LEVEL *lvl,unsigned int sx, unsigned int sy,unsigned char *src);
DLLIMPORT unsigned char *create_item(const struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned char stype_idx);

/* Categorization */
DLLIMPORT short is_nulltng(const unsigned char *thing);
DLLIMPORT short is_nulltng_stype(const unsigned char stype_idx);
DLLIMPORT int get_nulltng_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_nulltng_arritm(const int arr_itm);
DLLIMPORT unsigned int get_nulltng_count(void);
DLLIMPORT short is_effectgen(const unsigned char *thing);
DLLIMPORT int get_effectgen_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_effectgen_arritm(const int arr_itm);
DLLIMPORT unsigned int get_effectgen_count(void);
DLLIMPORT short is_itemeffect(const unsigned char *thing);
DLLIMPORT short is_itemeffect_stype(const unsigned char stype_idx);
DLLIMPORT int get_itemeffect_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_itemeffect_arritm(const int arr_itm);
DLLIMPORT unsigned int get_itemeffect_count(void);
DLLIMPORT short is_creature(const unsigned char *thing);
DLLIMPORT int get_creature_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_creature_arritm(const int arr_itm);
DLLIMPORT unsigned int get_creature_count(void);
DLLIMPORT short is_crtrlair(const unsigned char *thing);
DLLIMPORT short is_crtrlair_stype(const unsigned char stype_idx);
DLLIMPORT int get_crtrlair_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_crtrlair_arritm(const int arr_itm);
DLLIMPORT unsigned int get_crtrlair_count(void);
DLLIMPORT short is_trap(const unsigned char *thing);
DLLIMPORT int get_trap_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_trap_arritm(const int arr_itm);
DLLIMPORT unsigned int get_trap_count(void);
DLLIMPORT short is_door(const unsigned char *thing);
DLLIMPORT int get_door_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_door_arritm(const int arr_itm);
DLLIMPORT unsigned int get_door_count(void);
DLLIMPORT short is_dngspecbox(const unsigned char *thing);
DLLIMPORT short is_dngspecbox_stype(const unsigned char stype_idx);
DLLIMPORT int get_dngspecbox_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_dngspecbox_arritm(const int arr_itm);
DLLIMPORT unsigned int get_dngspecbox_count(void);
DLLIMPORT short is_spellbook(const unsigned char *thing);
DLLIMPORT short is_spellbook_stype(const unsigned char stype_idx);
DLLIMPORT int get_spellbook_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_spellbook_arritm(const int arr_itm);
DLLIMPORT unsigned int get_spellbook_count(void);
DLLIMPORT short is_wrkshopbox(const unsigned char *thing);
DLLIMPORT short is_wrkshopbox_stype(const unsigned char stype_idx);
DLLIMPORT int get_wrkshopbox_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_wrkshopbox_arritm(const int arr_itm);
DLLIMPORT unsigned int get_wrkshopbox_count(void);
DLLIMPORT short is_spinningtng(const unsigned char *thing);
DLLIMPORT short is_spinningtng_stype(const unsigned char stype_idx);
DLLIMPORT int get_spinningtng_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_spinningtng_arritm(const int arr_itm);
DLLIMPORT unsigned int get_spinningtng_count(void);
DLLIMPORT short is_food(const unsigned char *thing);
DLLIMPORT short is_food_stype(const unsigned char stype_idx);
DLLIMPORT int get_food_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_food_arritm(const int arr_itm);
DLLIMPORT unsigned int get_food_count(void);
DLLIMPORT short is_gold(const unsigned char *thing);
DLLIMPORT short is_gold_stype(const unsigned char stype_idx);
DLLIMPORT int get_gold_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_gold_arritm(const int arr_itm);
DLLIMPORT unsigned int get_gold_count(void);
DLLIMPORT short is_torchcndl(const unsigned char *thing);
DLLIMPORT short is_torchcndl_stype(const unsigned char stype_idx);
DLLIMPORT int get_torchcndl_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_torchcndl_arritm(const int arr_itm);
DLLIMPORT unsigned int get_torchcndl_count(void);
DLLIMPORT short is_heartflame(const unsigned char *thing);
DLLIMPORT short is_heartflame_stype(const unsigned char stype_idx);
DLLIMPORT int get_heartflame_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_heartflame_arritm(const int arr_itm);
DLLIMPORT unsigned char get_heartflame_next(const unsigned char stype_idx);
DLLIMPORT unsigned char get_heartflame_prev(const unsigned char stype_idx);
DLLIMPORT unsigned int get_heartflame_count(void);
DLLIMPORT short is_polebar(const unsigned char *thing);
DLLIMPORT short is_polebar_stype(const unsigned char stype_idx);
DLLIMPORT int get_polebar_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_polebar_arritm(const int arr_itm);
DLLIMPORT unsigned int get_polebar_count(void);
DLLIMPORT short is_statue(const unsigned char *thing);
DLLIMPORT short is_statue_stype(const unsigned char stype_idx);
DLLIMPORT int get_statue_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_statue_arritm(const int arr_itm);
DLLIMPORT unsigned int get_statue_count(void);
DLLIMPORT short is_furniture(const unsigned char *thing);
DLLIMPORT short is_furniture_stype(const unsigned char stype_idx);
DLLIMPORT int get_furniture_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_furniture_arritm(const int arr_itm);
DLLIMPORT unsigned int get_furniture_count(void);
DLLIMPORT short is_roomequip(const unsigned char *thing);
DLLIMPORT short is_roomequip_stype(const unsigned char stype_idx);
DLLIMPORT int get_roomequip_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_roomequip_arritm(const int arr_itm);
DLLIMPORT unsigned int get_roomequip_count(void);
DLLIMPORT short is_pwhand(const unsigned char *thing);
DLLIMPORT short is_pwhand_stype(const unsigned char stype_idx);
DLLIMPORT int get_pwhand_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_pwhand_arritm(const int arr_itm);
DLLIMPORT unsigned int get_pwhand_count(void);
DLLIMPORT short is_dncrucial(const unsigned char *thing);
DLLIMPORT short is_dncrucial_stype(const unsigned char stype_idx);
DLLIMPORT int get_dncrucial_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_dncrucial_arritm(const int arr_itm);
DLLIMPORT unsigned int get_dncrucial_count(void);
/* This one always returns false */
DLLIMPORT short is_false_stype(const unsigned char stype_idx);

/* Additional functions - not categorization */
DLLIMPORT short is_trapbox(const unsigned char *thing);
DLLIMPORT short is_trapbox_stype(const unsigned char stype_idx);
DLLIMPORT int get_trapbox_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_trapbox_arritm(const int arr_itm);
DLLIMPORT unsigned int get_trapbox_count(void);
DLLIMPORT short is_doorbox(const unsigned char *thing);
DLLIMPORT short is_doorbox_stype(const unsigned char stype_idx);
DLLIMPORT int get_doorbox_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_doorbox_arritm(const int arr_itm);
DLLIMPORT unsigned int get_doorbox_count(void);
DLLIMPORT short is_torch(const unsigned char *thing);
DLLIMPORT short is_torch_stype(const unsigned char stype_idx);
DLLIMPORT int get_torch_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_torch_arritm(const int arr_itm);
DLLIMPORT unsigned int get_torch_count(void);
DLLIMPORT short is_lit_thing(const unsigned char *thing);
DLLIMPORT short is_lit_thing_stype(const unsigned char stype_idx);
DLLIMPORT int get_lit_thing_arridx(const unsigned char stype_idx);
DLLIMPORT unsigned char get_lit_thing_arritm(const int arr_itm);
DLLIMPORT unsigned int get_lit_thing_count(void);

DLLIMPORT short is_herogate(const unsigned char *thing);
DLLIMPORT short is_dnheart(const unsigned char *thing);
DLLIMPORT short is_doorkey(const unsigned char *thing);
DLLIMPORT short is_trainpost(const unsigned char *thing);
DLLIMPORT short is_room_inventory(const unsigned char *thing);
DLLIMPORT short is_clmaffective_thing(const unsigned char *thing);

DLLIMPORT int get_item_category(unsigned char stype_idx);
DLLIMPORT unsigned char get_usual_item_slab(unsigned char stype_idx);
DLLIMPORT unsigned char get_usual_thing_slab(unsigned char *thing);
DLLIMPORT short subtl_in_thing_range(const unsigned char *thing,unsigned int sx,unsigned int sy);
unsigned long get_thing_distance_sqr_adv(const unsigned char *thing,const int ssx,const int ssy);
unsigned long get_thing_distance_adv(const unsigned char *thing,const int ssx,const int ssy);

DLLIMPORT char *get_thing_type_fullname(const unsigned short type_idx);
DLLIMPORT char *get_thing_type_shortname(const unsigned short type_idx);
DLLIMPORT char *get_item_subtype_fullname(const unsigned short stype_idx);
DLLIMPORT char *get_item_subtype_shortname(const unsigned short stype_idx);
DLLIMPORT char *get_creature_subtype_fullname(const unsigned short stype_idx);
DLLIMPORT char *get_creature_subtype_shortname(const unsigned short stype_idx);
DLLIMPORT char *get_trap_subtype_fullname(const unsigned short stype_idx);
DLLIMPORT char *get_door_subtype_fullname(const unsigned short stype_idx);
DLLIMPORT char *get_thing_category_fullname(const unsigned short arr_idx);
DLLIMPORT char *get_thing_category_shortname(const unsigned short arr_idx);
DLLIMPORT char *get_effectgen_subtype_fullname(const unsigned short stype_idx);
DLLIMPORT char *get_thing_subtype_fullname(const unsigned short type_idx,const unsigned short stype_idx);

DLLIMPORT int get_thing_subtypes_arridx(const unsigned char *thing);
DLLIMPORT unsigned int get_thing_subtypes_count(const unsigned short arr_idx);
DLLIMPORT unsigned int get_thing_subtypes_arritem(const unsigned short arr_idx,const int arr_itm);
DLLIMPORT int get_thing_subtypes_arritmidx(const unsigned short arr_idx,const unsigned short stype_idx);
DLLIMPORT thing_subtype_arrayitem get_thing_subtypes_arritem_func(const unsigned short arr_idx);
DLLIMPORT unsigned short get_thing_subtypes_next(const unsigned short arr_idx,const unsigned short stype_idx);
DLLIMPORT unsigned short get_thing_subtypes_prev(const unsigned short arr_idx,const unsigned short stype_idx);
DLLIMPORT short switch_thing_subtype(unsigned char *thing,const short forward);

short thing_verify(unsigned char *thing, struct VERIFY_OPTIONS *verif_opt);

#endif /* ADIKT_OBJTHINGS_H */
