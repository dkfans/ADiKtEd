/******************************************************************************/
/** @file obj_things.c
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Defines routines for maintaining single TNG entry - a thing.
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

#include "obj_things.h"

#include <math.h>
#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"
#include "lev_things.h"
#include "msg_log.h"

/*
Thing data block:
0x00 thing_subtpos_x
0x01 thing_subtile_x
0x02 thing_subtpos_y
0x03 thing_subtile_y
0x04 thing_subtpos_h
0x05 thing_subtile_h
0x06 thing_type
0x07 thing_subtype
0x08 thing_owner
0x09 thing_range_subtpos
0x0a thing_range_subtile
0x0b \
0x0c thing_sensitile/thing_index
0x0d door_orientation
0x0e thing_level/herogate_num/door_lock
0x0f
0x10
0x11
0x12
0x13
0x14
*/

const char *thing_fullnames[]={"Nothing", "Item/decoration", "unknown 2",
    "unknown 3", "unknown 4", "Creature", "unknown 6", "Effect Generatr",
    "Trap", "Door"};
const char *thing_shortnames[]={"empty", "thing", "unkn2",
    "unkn3", "unkn4", "crtur", "unkn6", "effct", "trap", "door"};
const char *traps[]={"", "Boulder", "Alarm", "Poison gas",
    "Lightning", "Word of power", "Lava", "Dummy 2", "Dummy 3",
    "Dummy 4", "Dummy 5", "Dummy 6", "Dummy 7"};
const char *doors[]={"", "Wooden", "Braced", "Iron", "Magic"};
const char *creature_names[]={"", "Wizrd", "Barb ", "Archr",
    "Monk ", "Dwarf", "Knigt", "Avtar", "Tunlr", "Witch",
    "Giant", "Fairy", "Thief", "Smuri", "Horny", "Skel ",
    "Troll", "Drgon", "Spawn", " Fly ", "DrkMs", "Warlk",
    "BileD", " Imp ", "Beetl", "Vamp ", "Spder", "Hound",
    "Ghost", "Tntcl", " Orc ", "Float"};
const char *creature_fullnames[]={"", "Wizard", "Barbarian",
    "Archer", "Monk", "Dwarf", "Knight", "Avatar", "Tunneller",
    "Witch", "Giant", "Fairy", "Thief", "Samurai", "Horned Reaper",
    "Skeleton", "Troll", "Dragon", "Demon Spawn", "Fly", "Dark Mistress",
    "Warlock", "Bile Demon", "Imp", "Beetle", "Vampire", "Spider",
    "Hell Hound", "Ghost", "Tentacle", "Orc", "Floating spirit"};
const char *effectgenrts[]={"unknown 0", "Lava", "Dripping water",
    "Rock Fall", "Entrance Ice", "Dry ice", "unknown 6", "unknown 7",
    "unknown 8"};

const unsigned char creatr_types[]={
      CREATR_SUBTP_WIZRD,CREATR_SUBTP_BARBARIN,CREATR_SUBTP_ARCHER,
      CREATR_SUBTP_MONK,CREATR_SUBTP_DWAFT,CREATR_SUBTP_KNIGHT,
      CREATR_SUBTP_AVATAR,CREATR_SUBTP_TUNELER,CREATR_SUBTP_WITCH,
      CREATR_SUBTP_GIANT,CREATR_SUBTP_FAIRY,CREATR_SUBTP_THEFT,
      CREATR_SUBTP_SMURI,CREATR_SUBTP_HORNY,CREATR_SUBTP_SKELETON,
      CREATR_SUBTP_TROLL,CREATR_SUBTP_DRAGON,CREATR_SUBTP_SPAWN,
      CREATR_SUBTP_FLY,CREATR_SUBTP_MISTRESS,CREATR_SUBTP_WARLOCK,
      CREATR_SUBTP_BILEDEMN,CREATR_SUBTP_IMP,CREATR_SUBTP_BEETLE,
      CREATR_SUBTP_VAMPIRE,CREATR_SUBTP_SPIDER,CREATR_SUBTP_HOUND,
      CREATR_SUBTP_GHOST,CREATR_SUBTP_TENTCL,CREATR_SUBTP_ORC,
      CREATR_SUBTP_FLOAT};

const unsigned char trap_types[]={
      TRAP_SUBTYPE_BOULDER,TRAP_SUBTYPE_ALARM,TRAP_SUBTYPE_GAS,
      TRAP_SUBTYPE_LIGHTNG,TRAP_SUBTYPE_WORDPWR,TRAP_SUBTYPE_LAVA,
      TRAP_SUBTYPE_DUMMY2,TRAP_SUBTYPE_DUMMY3,TRAP_SUBTYPE_DUMMY4,
      TRAP_SUBTYPE_DUMMY5,TRAP_SUBTYPE_DUMMY6,TRAP_SUBTYPE_DUMMY7};

const unsigned char door_types[]={
      DOOR_SUBTYPE_WOOD,DOOR_SUBTYPE_BRACED,
      DOOR_SUBTYPE_IRON,DOOR_SUBTYPE_MAGIC};

const unsigned char efctgen_types[]={
      EFCTGEN_SUBTP_LAVA,EFCTGEN_SUBTP_DRIPWTR,EFCTGEN_SUBTP_ROCKFAL,
      EFCTGEN_SUBTP_ENTRICE,EFCTGEN_SUBTP_DRYICE,
      };

/* Groups used for defining categories (every item is defined in exactly one group) */

const unsigned char items_nullthings[]={
      ITEM_SUBTYPE_NULL, ITEM_SUBTYPE_NULL1, ITEM_SUBTYPE_NULL2, };

const unsigned char items_effcts[]={
      ITEM_SUBTYPE_ARMOUR, ITEM_SUBTYPE_TEMPLESPN, ITEM_SUBTYPE_DISEASE, };

const unsigned char items_crtrlairs[]={
      ITEM_SUBTYPE_LAIRWIZRD,ITEM_SUBTYPE_LAIRBARBR,ITEM_SUBTYPE_LAIRARCHR,
      ITEM_SUBTYPE_LAIRMONK,ITEM_SUBTYPE_LAIRDWRFA,ITEM_SUBTYPE_LAIRKNGHT,
      ITEM_SUBTYPE_LAIRAVATR,ITEM_SUBTYPE_LAIRTUNLR,ITEM_SUBTYPE_LAIRWITCH,
      ITEM_SUBTYPE_LAIRGIANT,ITEM_SUBTYPE_LAIRFAIRY,ITEM_SUBTYPE_LAIRTHEFT,
      ITEM_SUBTYPE_LAIRSAMUR,ITEM_SUBTYPE_LAIRHORNY,ITEM_SUBTYPE_LAIRSKELT,
      ITEM_SUBTYPE_LAIRGOBLN,ITEM_SUBTYPE_LAIRDRAGN,ITEM_SUBTYPE_LAIRDEMSP,
      ITEM_SUBTYPE_LAIRFLY,ITEM_SUBTYPE_LAIRDKMIS,ITEM_SUBTYPE_LAIRSORCR,
      ITEM_SUBTYPE_LAIRBILDM,ITEM_SUBTYPE_LAIRIMP,ITEM_SUBTYPE_LAIRBUG,
      ITEM_SUBTYPE_LAIRVAMP,ITEM_SUBTYPE_LAIRSPIDR,ITEM_SUBTYPE_LAIRHLHND,
      ITEM_SUBTYPE_LAIRGHOST,ITEM_SUBTYPE_LAIRTENTC,ITEM_SUBTYPE_LAIRORC};

const unsigned char items_specboxes[]={
      ITEM_SUBTYPE_SPREVMAP,ITEM_SUBTYPE_SPRESURCT,ITEM_SUBTYPE_SPTRANSFR,
      ITEM_SUBTYPE_SPSTEALHR,ITEM_SUBTYPE_SPMULTPLY,ITEM_SUBTYPE_SPINCLEV,
      ITEM_SUBTYPE_SPMKSAFE,ITEM_SUBTYPE_SPHIDNWRL, ITEM_SUBTYPE_SPCUSTOM};

const unsigned char items_spellbooks[]={
      ITEM_SUBTYPE_SPELLHOE,ITEM_SUBTYPE_SPELLIMP,ITEM_SUBTYPE_SPELLMUST,
      ITEM_SUBTYPE_SPELLSLAP,ITEM_SUBTYPE_SPELLSOE,ITEM_SUBTYPE_SPELLCTA,
      ITEM_SUBTYPE_SPELLCAVI,ITEM_SUBTYPE_SPELLHEAL,ITEM_SUBTYPE_SPELLHLDA,
      ITEM_SUBTYPE_SPELLLIGH,ITEM_SUBTYPE_SPELLSPDC,ITEM_SUBTYPE_SPELLPROT,
      ITEM_SUBTYPE_SPELLCONC,ITEM_SUBTYPE_SPELLDISE,ITEM_SUBTYPE_SPELLCHKN,
      ITEM_SUBTYPE_SPELLDWAL,ITEM_SUBTYPE_SPELLTBMB,ITEM_SUBTYPE_SPELLARMG};

const unsigned char items_wrkshpbxs[]={
      ITEM_SUBTYPE_TBBOULDER,ITEM_SUBTYPE_TBALARM,ITEM_SUBTYPE_TBPOISONG,
      ITEM_SUBTYPE_TBLIGHTNG,ITEM_SUBTYPE_TBWRDOFPW,ITEM_SUBTYPE_TBLAVA,
      ITEM_SUBTYPE_TBDUMMY2,ITEM_SUBTYPE_TBDUMMY3,ITEM_SUBTYPE_TBDUMMY4,
      ITEM_SUBTYPE_TBDUMMY5,ITEM_SUBTYPE_TBDUMMY6,ITEM_SUBTYPE_TBDUMMY7,
      ITEM_SUBTYPE_DBWOOD,ITEM_SUBTYPE_DBBRACE,ITEM_SUBTYPE_DBSTEEL,
      ITEM_SUBTYPE_DBMAGIC, ITEM_SUBTYPE_WBITEM, };

const unsigned char items_spinnthings[]={
      ITEM_SUBTYPE_SPINNKEY, ITEM_SUBTYPE_SPINNKEY2,ITEM_SUBTYPE_SPINNCOIN, };

const unsigned char items_gold[]={
      ITEM_SUBTYPE_GOLDCHEST,ITEM_SUBTYPE_GOLD,ITEM_SUBTYPE_GOLDL,
      ITEM_SUBTYPE_GLDHOARD1,ITEM_SUBTYPE_GLDHOARD2,ITEM_SUBTYPE_GLDHOARD3,
      ITEM_SUBTYPE_GLDHOARD4,ITEM_SUBTYPE_GLDHOARD5, };

const unsigned char items_food[]={
      ITEM_SUBTYPE_CHICKNGRW, ITEM_SUBTYPE_CHICKN, ITEM_SUBTYPE_CHICKNSTB,
      ITEM_SUBTYPE_CHICKNWOB, ITEM_SUBTYPE_CHICKNCRK, };

const unsigned char items_torchcandls[]={
      ITEM_SUBTYPE_TORCH, ITEM_SUBTYPE_TORCHUN, ITEM_SUBTYPE_CANDLSTCK,};

const unsigned char items_heartflames[]={
      ITEM_SUBTYPE_HEARTFLMR, ITEM_SUBTYPE_HEARTFLMB, ITEM_SUBTYPE_HEARTFLMG,
      ITEM_SUBTYPE_HEARTFLMY,};

const unsigned char items_polebars[]={
      ITEM_SUBTYPE_GURDFLAGR,ITEM_SUBTYPE_GURDFLAGB,ITEM_SUBTYPE_GURDFLAGG,
      ITEM_SUBTYPE_GURDFLAGY,ITEM_SUBTYPE_FLAGPOST,ITEM_SUBTYPE_PRISONBAR,};

const unsigned char items_statues[]={
      ITEM_SUBTYPE_TEMPLESTA,ITEM_SUBTYPE_STATUEWO,ITEM_SUBTYPE_STATUHORN,
      ITEM_SUBTYPE_STATUE2,ITEM_SUBTYPE_STATUE3,ITEM_SUBTYPE_STATUE4,
      ITEM_SUBTYPE_STATUE5,};

const unsigned char items_furniture[]={
      ITEM_SUBTYPE_BARREL,ITEM_SUBTYPE_POTION1,ITEM_SUBTYPE_POTION2,
      ITEM_SUBTYPE_POTION3, ITEM_SUBTYPE_ANVIL, };

const unsigned char items_roomequip[]={
      ITEM_SUBTYPE_TRAINPOST, ITEM_SUBTYPE_GRAVSTONE, ITEM_SUBTYPE_SCAVNGEYE,
      ITEM_SUBTYPE_WRKSHPMCH, ITEM_SUBTYPE_TORTURER, ITEM_SUBTYPE_TORTSPIKE, };

const unsigned char items_pwhand[]={
      ITEM_SUBTYPE_PWHAND, ITEM_SUBTYPE_PWHANDGRB, ITEM_SUBTYPE_PWHANDWHP,
      ITEM_SUBTYPE_PWSIGHT, ITEM_SUBTYPE_PWLIGHTNG, ITEM_SUBTYPE_PWHANDGLD, };

const unsigned char items_dncrucial[]={
      ITEM_SUBTYPE_DNHEART, ITEM_SUBTYPE_HEROGATE, };

/* Other groups - used for different purposes than defining categories */
/* (one item may be defined in multiple groups here, or never be defined) */

const unsigned char items_litthings[]={
      ITEM_SUBTYPE_TORCH, ITEM_SUBTYPE_CANDLSTCK,};

const unsigned char items_trapbxs[]={
      ITEM_SUBTYPE_TBBOULDER,ITEM_SUBTYPE_TBALARM,ITEM_SUBTYPE_TBPOISONG,
      ITEM_SUBTYPE_TBLIGHTNG,ITEM_SUBTYPE_TBWRDOFPW,ITEM_SUBTYPE_TBLAVA,
      ITEM_SUBTYPE_TBDUMMY2,ITEM_SUBTYPE_TBDUMMY3,ITEM_SUBTYPE_TBDUMMY4,
      ITEM_SUBTYPE_TBDUMMY5,ITEM_SUBTYPE_TBDUMMY6,ITEM_SUBTYPE_TBDUMMY7};

const unsigned char items_doorboxes[]={
      ITEM_SUBTYPE_DBWOOD,ITEM_SUBTYPE_DBBRACE,ITEM_SUBTYPE_DBSTEEL,
      ITEM_SUBTYPE_DBMAGIC, };

const unsigned char items_torches[]={
      ITEM_SUBTYPE_TORCHUN, ITEM_SUBTYPE_TORCH, };

typedef unsigned int (*thing_subtype_counter)(void);

const is_thing_subtype thing_subtype_tests[]={
      is_nulltng,       is_effectgen,     is_itemeffect,
      is_creature,      is_crtrlair,      is_trap,
      is_door,          is_dngspecbox,    is_spellbook,
      is_wrkshopbox,    is_spinningtng,   is_food,
      is_gold,          is_torchcndl,     is_heartflame,
      is_polebar,       is_statue,        is_furniture,
      is_roomequip,     is_pwhand,        is_dncrucial,
      };

const is_item_subtype item_subtype_tests[]={
      is_nulltng_stype,       is_false_stype,         is_itemeffect_stype,
      is_false_stype,         is_crtrlair_stype,      is_false_stype,
      is_false_stype,         is_dngspecbox_stype,    is_spellbook_stype,
      is_wrkshopbox_stype,    is_spinningtng_stype,   is_food_stype,
      is_gold_stype,          is_torchcndl_stype,     is_heartflame_stype,
      is_polebar_stype,       is_statue_stype,        is_furniture_stype,
      is_roomequip_stype,     is_pwhand_stype,        is_dncrucial_stype,
      };

const char *thing_subtype_fullnames[]={
      "Null Things",       "Effect Generator",  "Item Effects",
      "Creatures",         "Creature Lairs",    "Traps",
      "Doors",             "Dung.Special boxes","Spell Books",       
      "Workshop Boxes",    "Spinning Things",   "Food (Chickens)",
      "Valuable (Gold)",   "Torches & Candles", "Heart Flames",
      "Poles & Bars",      "Statues",           "Furniture",
      "Room Equipment",    "Power Hands",       "Dungeon Crucial",
      };

const char *thing_subtype_shortnames[]={
      "Null Tngs", "EffectGen", "Itm Effct",
      "Creatures", "Lairs",     "Traps",
      "Doors",     "Dn.Specls", "Spl Books",       
      "WrkshpBxs", "Spinn.Tng", "Chickens",
      "Gold Tngs", "Trch/Cndl", "Heart Flm",
      "Pols/Bars", "Statues",   "Furniture",
      "RoomEquip", "Pwr Hands", "Dn.Crucil",
      };

const thing_subtype_counter thing_subtype_count[]={
      get_nulltng_count,       get_effectgen_count,     get_itemeffect_count,
      get_creature_count,      get_crtrlair_count,      get_trap_count,
      get_door_count,          get_dngspecbox_count,    get_spellbook_count,
      get_wrkshopbox_count,    get_spinningtng_count,   get_food_count,
      get_gold_count,          get_torchcndl_count,     get_heartflame_count,
      get_polebar_count,       get_statue_count,        get_furniture_count,
      get_roomequip_count,     get_pwhand_count,        get_dncrucial_count,
      };

const thing_subtype_arrayindex thing_subtype_arridx[]={
      get_nulltng_arridx,       get_effectgen_arridx,     get_itemeffect_arridx,
      get_creature_arridx,      get_crtrlair_arridx,      get_trap_arridx,
      get_door_arridx,          get_dngspecbox_arridx,    get_spellbook_arridx,
      get_wrkshopbox_arridx,    get_spinningtng_arridx,   get_food_arridx,
      get_gold_arridx,          get_torchcndl_arridx,     get_heartflame_arridx,
      get_polebar_arridx,       get_statue_arridx,        get_furniture_arridx,
      get_roomequip_arridx,     get_pwhand_arridx,        get_dncrucial_arridx,
      };

const thing_subtype_arrayitem thing_subtype_arritm[]={
      get_nulltng_arritm,       get_effectgen_arritm,     get_itemeffect_arritm,
      get_creature_arritm,      get_crtrlair_arritm,      get_trap_arritm,
      get_door_arritm,          get_dngspecbox_arritm,    get_spellbook_arritm,
      get_wrkshopbox_arritm,    get_spinningtng_arritm,   get_food_arritm,
      get_gold_arritm,          get_torchcndl_arritm,     get_heartflame_arritm,
      get_polebar_arritm,       get_statue_arritm,        get_furniture_arritm,
      get_roomequip_arritm,     get_pwhand_arritm,        get_dncrucial_arritm,
      };

unsigned char get_thing_type(const unsigned char *thing)
{
    if (thing==NULL) return THING_TYPE_NONE;
    return thing[6];
}

short set_thing_type(unsigned char *thing,unsigned char type_idx)
{
    if (thing==NULL) return false;
    thing[6]=type_idx;
    return true;
}

unsigned char get_thing_subtype(const unsigned char *thing)
{
    if (thing==NULL) return ITEM_SUBTYPE_NULL;
    return thing[7];
}

short set_thing_subtype(unsigned char *thing,const unsigned char stype_idx)
{
    if (thing==NULL) return false;
    thing[7]=stype_idx;
    return true;
}

unsigned char get_thing_owner(const unsigned char *thing)
{
    if (thing==NULL) return PLAYER_UNSET;
    if (thing[8]>=PLAYERS_COUNT) return PLAYER_UNSET;
    return thing[8];
}

short set_thing_owner(unsigned char *thing,unsigned char ownr_idx)
{
    if (thing==NULL) return false;
    thing[8]=ownr_idx;
    return true;
}

unsigned char get_thing_subtile_x(const unsigned char *thing)
{
    if (thing==NULL) return false;
    return (unsigned char)thing[1];
}

short set_thing_subtile_x(unsigned char *thing,unsigned char pos_x)
{
    if (thing==NULL) return false;
    thing[1]=pos_x;
    return true;
}

unsigned char get_thing_subtile_y(const unsigned char *thing)
{
    if (thing==NULL) return false;
    return (unsigned char)thing[3];
}

short set_thing_subtile_y(unsigned char *thing,unsigned char pos_y)
{
    if (thing==NULL) return false;
    thing[3]=pos_y;
    return true;
}

short set_thing_subtile(unsigned char *thing,unsigned char pos_x,unsigned char pos_y)
{
    if (thing==NULL) return false;
    thing[1]=pos_x;
    thing[3]=pos_y;
    return true;
}

unsigned char get_thing_subtile_h(const unsigned char *thing)
{
    if (thing==NULL) return 0;
    return (char)thing[5];
}

short set_thing_subtile_h(unsigned char *thing,unsigned char pos_h)
{
    if (thing==NULL) return false;
    thing[5]=pos_h;
    return true;
}

unsigned char get_thing_subtpos_x(const unsigned char *thing)
{
    if (thing==NULL) return 128;
    return (unsigned char)thing[0];
}

unsigned short get_thing_pos_x_adv(const unsigned char *thing)
{
    if (thing==NULL) return 0;
    return (((unsigned short)thing[1])<<8) + thing[0];
}

short set_thing_subtpos_x(unsigned char *thing,unsigned char pos_x)
{
    if (thing==NULL) return false;
    thing[0]=pos_x;
    return true;
}

unsigned char get_thing_subtpos_y(const unsigned char *thing)
{
    if (thing==NULL) return 0x80;
    return (char)thing[2];
}

unsigned short get_thing_pos_y_adv(const unsigned char *thing)
{
    if (thing==NULL) return 0;
    return (((unsigned short)thing[3])<<8) + thing[2];
}

short set_thing_subtpos_y(unsigned char *thing,unsigned char pos_y)
{
    if (thing==NULL) return false;
    thing[2]=pos_y;
    return true;
}

short set_thing_subtpos(unsigned char *thing,unsigned char pos_x,unsigned char pos_y)
{
    if (thing==NULL) return false;
    thing[0]=pos_x;
    thing[2]=pos_y;
    return true;
}

unsigned char get_thing_subtpos_h(const unsigned char *thing)
{
    if (thing==NULL) return 0x80;
    return (char)thing[4];
}

short set_thing_subtpos_h(unsigned char *thing,unsigned char pos_h)
{
    if (thing==NULL) return false;
    thing[4]=pos_h;
    return true;
}

unsigned char get_thing_range_subtpos(const unsigned char *thing)
{
    if (thing==NULL) return 0x80;
    return (char)thing[9];
}

short set_thing_range_subtpos(unsigned char *thing,unsigned char rng)
{
    if (thing==NULL) return false;
    thing[9]=rng;
    return true;
}

unsigned char get_thing_range_subtile(const unsigned char *thing)
{
    if (thing==NULL) return 0x80;
    return (char)thing[10];
}

short set_thing_range_subtile(unsigned char *thing,unsigned char rng)
{
    if (thing==NULL) return false;
    thing[10]=rng;
    return true;
}

/*
 * Returns range for any thing, as subtiles*256+within_subtile.
 */
unsigned int get_thing_range_adv(const unsigned char *thing)
{
  switch(get_thing_type(thing))
  {
    case THING_TYPE_ITEM:
      return (1<<8);
    case THING_TYPE_CREATURE:
      return (4<<8);
    case THING_TYPE_EFFECTGEN:
      return ((unsigned int)get_thing_range_subtile(thing)<<8)+get_thing_range_subtpos(thing);
    case THING_TYPE_TRAP:
      return (4<<8);
    case THING_TYPE_DOOR:
      return (4<<8);
    default:
      return 0;
  }
}

/*
 * Returns squared distance between given thing ang another map point.
 * It is very easy to exceed integer range with this function,
 * so it should be used only for short distances!
 * @param thing The thing which position defines first point.
 * @param ssx,ssy Coordinates of the second point ("_adv" form).
 * @return Returns squared distane in "_adv" form.
 */
unsigned long get_thing_distance_sqr_adv(const unsigned char *thing,const int ssx,const int ssy)
{
    int dx=get_thing_pos_x_adv(thing)-ssx;
    int dy=get_thing_pos_y_adv(thing)-ssy;
    return (dx*dx)+(dy*dy);
}

/*
 * Returns distance between given thing ang another map point.
 * Slower than get_thing_distance_sqr_adv, but it should fit integer range.
 * @param thing The thing which position defines first point.
 * @param ssx,ssy Coordinates of the second point ("_adv" form).
 * @return Returns squared distane in "_adv" form.
 */
unsigned long get_thing_distance_adv(const unsigned char *thing,const int ssx,const int ssy)
{
    int dx=get_thing_pos_x_adv(thing)-ssx;
    int dy=get_thing_pos_y_adv(thing)-ssy;
    float dist_sqr=((float)dx*dx)+((float)dy*dy);
    return sqrt(dist_sqr);
}

/*
 * Level is used as creature param, but also for numbering hero gates
 * and door lock state
 */
unsigned char get_thing_level(const unsigned char *thing)
{
    if (thing==NULL) return 0;
    return thing[14];
}

short set_thing_level(unsigned char *thing,unsigned char lev_num)
{
    if (thing==NULL) return false;
    thing[14]=lev_num;
    return true;
}

/*
 * For some things, this is a number of tile (not subtile) at wchich the thing is
 */
unsigned short get_thing_sensitile(const unsigned char *thing)
{
    if (thing==NULL) return THING_SENSITILE_NONE;
    return (unsigned short)((thing[12]<<8)+thing[11]);
}

/*
 * For some things, this is a number of tile (not subtile) at which the thing is
 */
short set_thing_sensitile(unsigned char *thing,unsigned short til_num)
{
    if (thing==NULL) return false;
    thing[12]=(til_num>>8)&255;
    thing[11]=til_num&255;
    return true;
}

unsigned char get_door_orientation(const unsigned char *thing)
{
    if (thing==NULL) return DOOR_ORIENT_NSPASS;
    return thing[13];
}

short set_door_orientation(unsigned char *thing,unsigned char orient)
{
    if (thing==NULL) return false;
    thing[13]=orient;
    return true;
}

short switch_thing_subtype(unsigned char *thing,const short forward)
{
    int i=get_thing_subtypes_arridx(thing);
    if (i<0)
      return false;
    unsigned char stype_idx=get_thing_subtype(thing);
    unsigned char stype_new;
    if (forward)
      stype_new=get_thing_subtypes_next(i,stype_idx);
    else
      stype_new=get_thing_subtypes_prev(i,stype_idx);
    if (stype_new!=stype_idx)
    {
      set_thing_subtype(thing,stype_new);
      return true;
    }
    return false;
}

int get_thing_subtypes_arridx(const unsigned char *thing)
{
    if (thing==NULL) return -1;
    int func_count=sizeof(thing_subtype_tests)/sizeof(is_thing_subtype);
    int i;
    for (i=0;i<func_count;i++)
    {
      if (thing_subtype_tests[i](thing))
        return i;
    }
    return -1;
}

/*
 * Return number of items in the subtypes array indexed arr_idx.
 */
unsigned int get_thing_subtypes_count(const unsigned short arr_idx)
{
    const int func_count=sizeof(thing_subtype_arritm)/sizeof(thing_subtype_arrayitem);
    if (arr_idx<func_count)
        return thing_subtype_count[arr_idx]();
    return 0;
}

/*
 * Return value of item at position arr_itm in the subtypes array indexed arr_idx.
 */
unsigned int get_thing_subtypes_arritem(const unsigned short arr_idx,const int arr_itm)
{
    const int func_count=sizeof(thing_subtype_arritm)/sizeof(thing_subtype_arrayitem);
    if (arr_idx<func_count)
        return thing_subtype_arritm[arr_idx](arr_itm);
    return 0;
}

/*
 * Return position of item stype_idx in the subtypes array indexed arr_idx.
 */
int get_thing_subtypes_arritmidx(const unsigned short arr_idx,const unsigned short stype_idx)
{
    const int func_count=sizeof(thing_subtype_arritm)/sizeof(thing_subtype_arrayitem);
    if (arr_idx<func_count)
        return thing_subtype_arridx[arr_idx](stype_idx);
    return -1;
}

/*
 * Returns function for retrieving value of item in the subtypes array indexed arr_idx.
 */
thing_subtype_arrayitem get_thing_subtypes_arritem_func(const unsigned short arr_idx)
{
    const int func_count=sizeof(thing_subtype_arritm)/sizeof(thing_subtype_arrayitem);
    if (arr_idx<func_count)
        return thing_subtype_arritm[arr_idx];
    return NULL;
}

/*
 * Return next subtype after given stype_idx in the subtypes array indexed arr_idx.
 */
unsigned short get_thing_subtypes_next(const unsigned short arr_idx,const unsigned short stype_idx)
{
    const int func_count=sizeof(thing_subtype_arritm)/sizeof(thing_subtype_arrayitem);
    if (arr_idx>=func_count)
        return stype_idx;
    int prev_idx;
    prev_idx=thing_subtype_arridx[arr_idx](stype_idx);
    if (prev_idx<0)
        return stype_idx;
    return thing_subtype_arritm[arr_idx](prev_idx+1);
}

/*
 * Return previous subtype before given stype_idx in the subtypes array indexed arr_idx.
 */
unsigned short get_thing_subtypes_prev(const unsigned short arr_idx,const unsigned short stype_idx)
{
    const int func_count=sizeof(thing_subtype_arritm)/sizeof(thing_subtype_arrayitem);
    if (arr_idx>=func_count)
        return stype_idx;
    int prev_idx;
    prev_idx=thing_subtype_arridx[arr_idx](stype_idx);
    if (prev_idx<0)
        return stype_idx;
    return thing_subtype_arritm[arr_idx](prev_idx-1);
}

char *get_thing_type_fullname(unsigned short type_idx)
{
     int types_count=sizeof(thing_fullnames)/sizeof(char *);
     if (type_idx<types_count)
       return (char *)thing_fullnames[type_idx];
     else
       return "unknown(?!)";
}

char *get_thing_type_shortname(const unsigned short type_idx)
{
     int types_count=sizeof(thing_shortnames)/sizeof(char *);
     if (type_idx<types_count)
       return (char *)thing_shortnames[type_idx];
     else
       return "unkn!";
}

char *get_thing_subtype_fullname(const unsigned short type_idx,const unsigned short stype_idx)
{
  static char buffer[LINEMSG_SIZE];
  switch (type_idx)
  {
  case THING_TYPE_ITEM:
      sprintf(buffer,"%s",get_item_subtype_fullname(stype_idx));
      break;
  case THING_TYPE_CREATURE:
      sprintf(buffer,"%s",get_creature_subtype_fullname(stype_idx));
      break;
  case THING_TYPE_EFFECTGEN:
      sprintf(buffer,"%s effect",get_effectgen_subtype_fullname(stype_idx));
      break;
  case THING_TYPE_TRAP:
      sprintf(buffer,"%s trap",get_trap_subtype_fullname(stype_idx));
      break;
  case THING_TYPE_DOOR:
      sprintf(buffer,"%s door",get_door_subtype_fullname(stype_idx));
      break;
  default:
      sprintf(buffer,"%s",get_thing_type_fullname(type_idx));
      break;
  }
  return buffer;
}

/*
 * Returns SLB entry on which the thing is usually placed.
 * Note that if SLAB_TYPE_CLAIMED is returned, this means
 * a thing can be found on any short slab (all rooms,water,path,...)
 */
unsigned char get_usual_thing_slab(unsigned char *thing)
{
  if (thing==NULL) return SLAB_TYPE_CLAIMED;
  unsigned char type_idx=get_thing_type(thing);
  unsigned char stype_idx=get_thing_subtype(thing);
  switch (type_idx)
  {
  case THING_TYPE_CREATURE:
      return SLAB_TYPE_CLAIMED;
  case THING_TYPE_TRAP:
      return SLAB_TYPE_CLAIMED;
  case THING_TYPE_ITEM:
      return get_usual_item_slab(stype_idx);
  case THING_TYPE_EFFECTGEN:
      {
        switch (stype_idx)
        {
        case EFCTGEN_SUBTP_LAVA:
            return SLAB_TYPE_LAVA;
        case EFCTGEN_SUBTP_DRIPWTR:
            /*return SLAB_TYPE_WATER; */
            /*Changed because this is sometimes placed as "user-made effect" */
            /* (usually on lava) */
            return SLAB_TYPE_CLAIMED;
        case EFCTGEN_SUBTP_ROCKFAL:
            return SLAB_TYPE_PATH;
        case EFCTGEN_SUBTP_ENTRICE:
            /*return SLAB_TYPE_PORTAL; */
            /*Changed because this is sometimes placed as "user-made effect" */
            /* (usually on hero gates) */
            return SLAB_TYPE_CLAIMED;
        case EFCTGEN_SUBTP_DRYICE:
            /*return SLAB_TYPE_GRAVEYARD; */
            /*Changed because this is sometimes placed as "user-made effect" */
            /*(on dyngeon specials and in other places) */
            return SLAB_TYPE_CLAIMED;
        }
      };break;
  case THING_TYPE_DOOR:
      {
        switch (stype_idx)
        {
        case DOOR_SUBTYPE_WOOD:
            return SLAB_TYPE_DOORWOOD1;
        case DOOR_SUBTYPE_BRACED:
            return SLAB_TYPE_DOORBRACE1;
        case DOOR_SUBTYPE_IRON:
            return SLAB_TYPE_DOORIRON1;
        case DOOR_SUBTYPE_MAGIC:
            return SLAB_TYPE_DOORMAGIC1;
        }
      };break;
  }
  return SLAB_TYPE_CLAIMED;
}

/*
 * Returns SLB entry on which the item is usually placed.
 * Note that if SLAB_TYPE_CLAIMED is returned, this means
 * a thing can be found on any short slab (all rooms,water,path,...)
 */
unsigned char get_usual_item_slab(unsigned char stype_idx)
{
  int catg=get_item_category(stype_idx);
  switch (catg)
  {
  case THING_CATEGR_CREATLAIR:
      return SLAB_TYPE_LAIR;
  case THING_CATEGR_FOOD:
      return SLAB_TYPE_HATCHERY;
  case THING_CATEGR_TORCHCNDL:
      return SLAB_TYPE_CLAIMED;
  case THING_CATEGR_HEARTFLAME:
      return SLAB_TYPE_DUNGHEART;
  case THING_CATEGR_ITEMEFFCT:
  case THING_CATEGR_POLEBAR:
  case THING_CATEGR_FURNITURE:
  case THING_CATEGR_ROOMEQUIP:
  case THING_CATEGR_DNCRUCIAL:
    {
    /*TODO: validate when all rooms will be done */
        switch (stype_idx)
        {
        case ITEM_SUBTYPE_DNHEART:
          return SLAB_TYPE_DUNGHEART;
        case ITEM_SUBTYPE_HEROGATE:
          return SLAB_TYPE_CLAIMED;
        case ITEM_SUBTYPE_FLAGPOST:
        case ITEM_SUBTYPE_GURDFLAGR:
        case ITEM_SUBTYPE_GURDFLAGB:
        case ITEM_SUBTYPE_GURDFLAGG:
        case ITEM_SUBTYPE_GURDFLAGY:
          return SLAB_TYPE_GUARDPOST;
        /*TODO: not sure about these */
        case ITEM_SUBTYPE_PRISONBAR:
          return SLAB_TYPE_PRISONCASE;
        case ITEM_SUBTYPE_SCAVNGEYE:
          return SLAB_TYPE_SCAVENGER;
        case ITEM_SUBTYPE_ANVIL:
        case ITEM_SUBTYPE_WRKSHPMCH:
          return SLAB_TYPE_WORKSHOP;
        case ITEM_SUBTYPE_GRAVSTONE:
          return SLAB_TYPE_GRAVEYARD;
        case ITEM_SUBTYPE_TRAINPOST:
          return SLAB_TYPE_TRAINING;
        case ITEM_SUBTYPE_TORTSPIKE:
        case ITEM_SUBTYPE_TORTURER:
          return SLAB_TYPE_TORTURE;
        }
    };break;
  }
  return SLAB_TYPE_CLAIMED;
}

/*
 * Creates a new thing, completely empty one,
 * without placing it into LEVEL structure
 */
unsigned char *create_thing_empty(void)
{
    unsigned char *thing;
    thing = (unsigned char *)malloc(SIZEOF_DK_TNG_REC);
    if (thing==NULL)
    {
        message_error("create_thing: Out of memory.");
        return NULL;
    }
    return thing;
}

/*
 * Creates a new thing, prepared to be placed in given coordinates,
 * but without placing it into LEVEL structure
 */
unsigned char *create_thing(unsigned int sx, unsigned int sy)
{
    unsigned char *thing;
    thing = create_thing_empty();
    int i;
    for (i=0; i < SIZEOF_DK_TNG_REC; i++)
      thing[i]=0;
    set_thing_subtpos(thing,((sx%MAP_SUBNUM_X)*0x40+0x40),((sy%MAP_SUBNUM_Y)*0x40+0x40));
    set_thing_subtile(thing,(unsigned char)sx,(unsigned char)sy);
    set_thing_subtile_h(thing,1); /* Default height is floor height */
    /* Setting the owner */
    set_thing_owner(thing,PLAYER_UNSET);
    return thing;
}

/*
 * Creates a new thing, prepared to be placed in given coordinates,
 * with body copied from another thing, given as parameter.
 */
unsigned char *create_thing_copy(const struct LEVEL *lvl,unsigned int sx, unsigned int sy,unsigned char *src)
{
    /*Preparing array bounds */
    const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    sx%=arr_entries_x;
    sy%=arr_entries_y;

    unsigned char *thing;
    thing = create_thing_empty();
    memcpy(thing,src,SIZEOF_DK_TNG_REC);
    unsigned char type_idx=get_thing_type(thing);
    /*unsigned char stype_idx=get_thing_subtype(thing);*/
    set_thing_subtpos(thing,((sx%MAP_SUBNUM_X)*0x40+0x40),((sy%MAP_SUBNUM_Y)*0x40+0x40));
    set_thing_subtile(thing,(unsigned char)sx,(unsigned char)sy);
    if (type_idx==THING_TYPE_EFFECTGEN)
      set_thing_sensitile(thing,compute_effectgen_sensitile(lvl,thing));
    if (type_idx==THING_TYPE_ITEM)
      set_thing_sensitile(thing,compute_item_sensitile(lvl,thing));
    return thing;
}

/*
 * Creates a new thing of type item
 */
unsigned char *create_item(const struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    set_thing_type(thing,THING_TYPE_ITEM);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,tx,ty));
    set_thing_sensitile(thing,THING_SENSITILE_NONE); /*Default associated tile is none */
    return thing;
}

/*
 * Returns if the thing is a spellbook
 */
short is_spellbook(const unsigned char *thing)
{
    /*All spells are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_spellbook_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing is a spellbook
 */
short is_spellbook_stype(const unsigned char stype_idx)
{
    if (get_spellbook_arridx(stype_idx)<0) return false;
    return true;
}

int get_spellbook_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_spellbooks;
    int array_count=get_spellbook_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_spellbook_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_spellbooks;
    int array_count=get_spellbook_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_spellbook_count(void)
{
     return sizeof(items_spellbooks)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next spellbook
 */
unsigned char get_spellbook_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_spellbooks)/sizeof(unsigned char);
    /*find the spell in items_spellbooks array */
    unsigned char *pos=memchr(items_spellbooks,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_spellbooks)||(pos>=items_spellbooks+sizeof(items_spellbooks)))
      pos=(unsigned char *)items_spellbooks;
    return *pos;
}

/*
 * Returns subtype of a previous spellbook
 */
unsigned char get_spellbook_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_spellbooks)/sizeof(unsigned char);
    /*find the spell in items_spellbooks array */
    unsigned char *pos=memchr(items_spellbooks,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_spellbooks)||(pos>=items_spellbooks+sizeof(items_spellbooks)))
      pos=(unsigned char *)items_spellbooks+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a dungeon special box
 */
short is_dngspecbox(const unsigned char *thing)
{
    /*All specials are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_dngspecbox_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a dungeon special box
 */
short is_dngspecbox_stype(const unsigned char stype_idx)
{
    if (get_dngspecbox_arridx(stype_idx)<0) return false;
    return true;
}

int get_dngspecbox_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_specboxes;
    int array_count=get_dngspecbox_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_dngspecbox_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_specboxes;
    int array_count=get_dngspecbox_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_dngspecbox_count(void)
{
     return sizeof(items_specboxes)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next dungeon special box
 */
unsigned char get_dngspecbox_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_specboxes)/sizeof(unsigned char);
    /*find the special in items_specboxes array */
    unsigned char *pos=memchr(items_specboxes,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_specboxes)||(pos>=items_specboxes+sizeof(items_specboxes)))
      pos=(unsigned char *)items_specboxes;
    return *pos;
}

/*
 * Returns subtype of a previous dungeon special box
 */
unsigned char get_dngspecbox_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_specboxes)/sizeof(unsigned char);
    /*find the special in items_specboxes array */
    unsigned char *pos=memchr(items_specboxes,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_specboxes)||(pos>=items_specboxes+sizeof(items_specboxes)))
      pos=(unsigned char *)items_specboxes+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a creature lair
 */
short is_crtrlair(const unsigned char *thing)
{
    /*All lairs are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_crtrlair_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a creature lair
 */
short is_crtrlair_stype(const unsigned char stype_idx)
{
    if (get_crtrlair_arridx(stype_idx)<0) return false;
    return true;
}

int get_crtrlair_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_crtrlairs;
    int array_count=get_crtrlair_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_crtrlair_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_crtrlairs;
    int array_count=get_crtrlair_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_crtrlair_count(void)
{
     return sizeof(items_crtrlairs)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next creature lair
 */
unsigned char get_crtrlair_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_crtrlairs)/sizeof(unsigned char);
    /*find the lair in items_crtrlairs array */
    unsigned char *pos=memchr(items_crtrlairs,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_crtrlairs)||(pos>=items_crtrlairs+sizeof(items_crtrlairs)))
      pos=(unsigned char *)items_crtrlairs;
    return *pos;
}

/*
 * Returns subtype of a previous creature lair
 */
unsigned char get_crtrlair_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_crtrlairs)/sizeof(unsigned char);
    /*find the lair in items_crtrlairs array */
    unsigned char *pos=memchr(items_crtrlairs,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_crtrlairs)||(pos>=items_crtrlairs+sizeof(items_crtrlairs)))
      pos=(unsigned char *)items_crtrlairs+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a trap box
 */
short is_trapbox(const unsigned char *thing)
{
    /*All traps are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_trapbox_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a trap box
 */
short is_trapbox_stype(const unsigned char stype_idx)
{
    if (get_trapbox_arridx(stype_idx)<0) return false;
    return true;
}

int get_trapbox_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_trapbxs;
    int array_count=get_trapbox_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_trapbox_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_trapbxs;
    int array_count=get_trapbox_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_trapbox_count(void)
{
     return sizeof(items_trapbxs)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next trap box
 */
unsigned char get_trapbox_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_trapbxs)/sizeof(unsigned char);
    /*find the trap box in items_trapbxs array */
    unsigned char *pos=memchr(items_trapbxs,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_trapbxs)||(pos>=items_trapbxs+sizeof(items_trapbxs)))
      pos=(unsigned char *)items_trapbxs;
    return *pos;
}

/*
 * Returns subtype of a previous trap box
 */
unsigned char get_trapbox_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_trapbxs)/sizeof(unsigned char);
    /*find the trap box in items_trapbxs array */
    unsigned char *pos=memchr(items_trapbxs,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_trapbxs)||(pos>=items_trapbxs+sizeof(items_trapbxs)))
      pos=(unsigned char *)items_trapbxs+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a deployed trap
 */
short is_trap(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_TRAP)
      return true;
    return false;
}

int get_trap_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)trap_types;
    int array_count=get_trap_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_trap_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)trap_types;
    int array_count=get_trap_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_trap_count(void)
{
     return sizeof(trap_types)/sizeof(unsigned char);
}

/*
 * Returns if the thing is a creature
 */
short is_creature(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_CREATURE)
      return true;
    return false;
}

int get_creature_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)creatr_types;
    int array_count=get_creature_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_creature_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)creatr_types;
    int array_count=get_creature_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_creature_count(void)
{
     return sizeof(creatr_types)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next creature kind
 */
unsigned char get_creature_next(const unsigned char stype_idx)
{
     int array_count=sizeof(creatr_types)/sizeof(unsigned char);
    /*find the creature in creatr_types array */
    unsigned char *pos=memchr(creatr_types,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<creatr_types)||(pos>=creatr_types+sizeof(creatr_types)))
      pos=(unsigned char *)creatr_types;
    return *pos;
}

/*
 * Returns subtype of a previous creature kind
 */
unsigned char get_creature_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(creatr_types)/sizeof(unsigned char);
    /*find the creature in creatr_types array */
    unsigned char *pos=memchr(creatr_types,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<creatr_types)||(pos>=creatr_types+sizeof(creatr_types)))
      pos=(unsigned char *)creatr_types+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a door box
 */
short is_doorbox(const unsigned char *thing)
{
    /*All traps are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_doorbox_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a trap box
 */
short is_doorbox_stype(const unsigned char stype_idx)
{
    if (get_doorbox_arridx(stype_idx)<0) return false;
    return true;
}

int get_doorbox_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_doorboxes;
    int array_count=get_doorbox_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_doorbox_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_doorboxes;
    int array_count=get_doorbox_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_doorbox_count(void)
{
     return sizeof(items_doorboxes)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next trap box
 */
unsigned char get_doorbox_next(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_doorboxes;
    int sizeof_arr=sizeof(items_doorboxes);
    int array_count=sizeof_arr/sizeof(unsigned char);
    /*find the item in array */
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos++;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr;
    return *pos;
}

/*
 * Returns subtype of a previous trap box
 */
unsigned char get_doorbox_prev(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_doorboxes;
    int sizeof_arr=sizeof(items_doorboxes);
    int array_count=sizeof_arr/sizeof(unsigned char);
    /*find the item in array */
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos--;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr;
    return *pos;
}

/*
 * Returns if the thing is a door
 */
short is_door(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_DOOR)
      return true;
    return false;
}

int get_door_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)door_types;
    int array_count=get_door_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_door_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)door_types;
    int array_count=get_door_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_door_count(void)
{
     return sizeof(door_types)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next door type
 */
unsigned char get_door_next(const unsigned char stype_idx)
{
     int array_count=sizeof(door_types)/sizeof(unsigned char);
    /*find the door in door_types array */
    unsigned char *pos=memchr(door_types,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<door_types)||(pos>=door_types+sizeof(door_types)))
      pos=(unsigned char *)door_types;
    return *pos;
}

/*
 * Returns subtype of a previous door type
 */
unsigned char get_door_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(door_types)/sizeof(unsigned char);
    /*find the door in door_types array */
    unsigned char *pos=memchr(door_types,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<door_types)||(pos>=door_types+sizeof(door_types)))
      pos=(unsigned char *)door_types+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a effect generator
 */
short is_effectgen(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_EFFECTGEN)
      return true;
    return false;
}

int get_effectgen_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)efctgen_types;
    int array_count=get_effectgen_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_effectgen_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)efctgen_types;
    int array_count=get_effectgen_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_effectgen_count(void)
{
     return sizeof(efctgen_types)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next effect generator type
 */
unsigned char get_effectgen_next(const unsigned char stype_idx)
{
     int array_count=sizeof(efctgen_types)/sizeof(unsigned char);
    /*find the effect in efctgen_types array */
    unsigned char *pos=memchr(efctgen_types,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<efctgen_types)||(pos>=efctgen_types+sizeof(efctgen_types)))
      pos=(unsigned char *)efctgen_types;
    return *pos;
}

/*
 * Returns subtype of a previous effect generator type
 */
unsigned char get_effectgen_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(efctgen_types)/sizeof(unsigned char);
    /*find the effect in efctgen_types array */
    unsigned char *pos=memchr(efctgen_types,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<efctgen_types)||(pos>=efctgen_types+sizeof(efctgen_types)))
      pos=(unsigned char *)efctgen_types+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a statue
 */
short is_statue(const unsigned char *thing)
{
    /*All statues are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_statue_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a statue
 */
short is_statue_stype(const unsigned char stype_idx)
{
    if (get_statue_arridx(stype_idx)<0) return false;
    return true;
}

int get_statue_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_statues;
    int array_count=get_statue_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_statue_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_statues;
    int array_count=get_statue_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_statue_count(void)
{
     return sizeof(items_statues)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next statue
 */
unsigned char get_statue_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_statues)/sizeof(unsigned char);
    /*find the statue in items_statues array */
    unsigned char *pos=memchr(items_statues,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_statues)||(pos>=items_statues+sizeof(items_statues)))
      pos=(unsigned char *)items_statues;
    return *pos;
}

/*
 * Returns subtype of a previous statue
 */
unsigned char get_statue_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_statues)/sizeof(unsigned char);
    /*find the statue in items_statues array */
    unsigned char *pos=memchr(items_statues,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_statues)||(pos>=items_statues+sizeof(items_statues)))
      pos=(unsigned char *)items_statues+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a food
 */
short is_food(const unsigned char *thing)
{
    /*All food are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_food_stype(get_thing_subtype(thing));
}

/*
 * Returns if the subtype is a food
 */
short is_food_stype(const unsigned char stype_idx)
{
    if (get_food_arridx(stype_idx)<0) return false;
    return true;
}

int get_food_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_food;
    int array_count=get_food_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_food_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_food;
    int array_count=get_food_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_food_count(void)
{
     return sizeof(items_food)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next food
 */
unsigned char get_food_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_food)/sizeof(unsigned char);
    /*find the food in items_food array */
    unsigned char *pos=memchr(items_food,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_food)||(pos>=items_food+sizeof(items_food)))
      pos=(unsigned char *)items_food;
    return *pos;
}

/*
 * Returns subtype of a previous food
 */
unsigned char get_food_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_food)/sizeof(unsigned char);
    /*find the food in items_food array */
    unsigned char *pos=memchr(items_food,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_food)||(pos>=items_food+sizeof(items_food)))
      pos=(unsigned char *)items_food+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

short is_gold(const unsigned char *thing)
{
    /*All are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_gold_stype(get_thing_subtype(thing));
}

short is_gold_stype(const unsigned char stype_idx)
{
    if (get_gold_arridx(stype_idx)<0) return false;
    return true;
}

int get_gold_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_gold;
    int array_count=get_gold_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_gold_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_gold;
    int array_count=get_gold_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_gold_count(void)
{
     return sizeof(items_gold)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next gold
 */
unsigned char get_gold_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_gold)/sizeof(unsigned char);
    /*find the gold in items_gold array */
    unsigned char *pos=memchr(items_gold,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_gold)||(pos>=items_gold+sizeof(items_gold)))
      pos=(unsigned char *)items_gold;
    return *pos;
}

/*
 * Returns subtype of a previous gold
 */
unsigned char get_gold_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_gold)/sizeof(unsigned char);
    /*find the gold in items_gold array */
    unsigned char *pos=memchr(items_gold,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_gold)||(pos>=items_gold+sizeof(items_gold)))
      pos=(unsigned char *)items_gold+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a torch
 */
short is_torch(const unsigned char *thing)
{
    /*All torches are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_torch_stype(get_thing_subtype(thing));
}

short is_torch_stype(const unsigned char stype_idx)
{
    if (get_torch_arridx(stype_idx)<0) return false;
    return true;
}

int get_torch_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_torches;
    int array_count=get_torch_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_torch_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_torches;
    int array_count=get_torch_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_torch_count(void)
{
     return sizeof(items_torches)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next torch
 */
unsigned char get_torch_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_torches)/sizeof(unsigned char);
    /*find the torch in items_torches array */
    unsigned char *pos=memchr(items_torches,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_torches)||(pos>=items_torches+sizeof(items_torches)))
      pos=(unsigned char *)items_torches;
    return *pos;
}

/*
 * Returns subtype of a previous torch
 */
unsigned char get_torch_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_torches)/sizeof(unsigned char);
    /*find the torch in items_torches array */
    unsigned char *pos=memchr(items_torches,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_torches)||(pos>=items_torches+sizeof(items_torches)))
      pos=(unsigned char *)items_torches+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a heartflame
 */
short is_heartflame(const unsigned char *thing)
{
    /*All heartflames are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_heartflame_stype(get_thing_subtype(thing));
}

short is_heartflame_stype(const unsigned char stype_idx)
{
    if (get_heartflame_arridx(stype_idx)<0) return false;
    return true;
}

int get_heartflame_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_heartflames;
    int array_count=get_heartflame_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_heartflame_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_heartflames;
    int array_count=get_heartflame_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_heartflame_count(void)
{
     return sizeof(items_heartflames)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next heartflame
 */
unsigned char get_heartflame_next(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_heartflames;
    int sizeof_arr=sizeof(items_heartflames);
    int sizeof_itm=sizeof(unsigned char);
    int array_count=sizeof_arr/sizeof_itm;
    /*find the item in array */
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof_itm;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr;
    return *pos;
}

/*
 * Returns subtype of a previous heartflame
 */
unsigned char get_heartflame_prev(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_heartflames;
    int sizeof_arr=sizeof(items_heartflames);
    int array_count=sizeof_arr/sizeof(unsigned char);
    /*find the item in array */
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos--;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr+(array_count-1);
    return *pos;
}

/*
 * Returns if the thing is a flag pole or prison bar
 */
short is_polebar(const unsigned char *thing)
{
    /*All poles are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_polebar_stype(get_thing_subtype(thing));
}

short is_polebar_stype(const unsigned char stype_idx)
{
    if (get_polebar_arridx(stype_idx)<0) return false;
    return true;
}

int get_polebar_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_polebars;
    int array_count=get_polebar_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_polebar_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_polebars;
    int array_count=get_polebar_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_polebar_count(void)
{
     return sizeof(items_polebars)/sizeof(unsigned char);
}

/*
 * Returns true if the thing has a light (is lit thing)
 */
short is_lit_thing(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ITEM)
      return is_lit_thing_stype(get_thing_subtype(thing));
    return false;
}

short is_lit_thing_stype(const unsigned char stype_idx)
{
    if (get_lit_thing_arridx(stype_idx)<0) return false;
    return true;
}

int get_lit_thing_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_litthings;
    int array_count=get_lit_thing_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_lit_thing_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_litthings;
    int array_count=get_lit_thing_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_lit_thing_count(void)
{
     return sizeof(items_litthings)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next lit thing
 */
unsigned char get_lit_thing_next(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_litthings;
    int sizeof_arr=sizeof(items_litthings);
    int sizeof_itm=sizeof(unsigned char);
    int array_count=sizeof_arr/sizeof_itm;
    /*find the item in array */
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof_itm;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr;
    return *pos;
}

/*
 * Returns subtype of a previous lit thing
 */
unsigned char get_lit_thing_prev(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_litthings;
    int sizeof_arr=sizeof(items_litthings);
    int array_count=sizeof_arr/sizeof(unsigned char);
    /*find the item in array */
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos--;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr+(array_count-1);
    return *pos;
}

/*
 * Returns true if the thing is a spinning thing
 */
short is_spinningtng(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ITEM)
      return is_spinningtng_stype(get_thing_subtype(thing));
    return false;
}

short is_spinningtng_stype(const unsigned char stype_idx)
{
    if (get_spinningtng_arridx(stype_idx)<0) return false;
    return true;
}

int get_spinningtng_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_spinnthings;
    int array_count=get_spinningtng_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

/*
 * returns item on speficied index in the thing type array
 */
unsigned char get_spinningtng_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_spinnthings;
    int array_count=get_spinningtng_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_spinningtng_count(void)
{
     return sizeof(items_spinnthings)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next spinning thing
 */
unsigned char get_spinningtng_next(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_spinnthings;
    int sizeof_arr=sizeof(items_spinnthings);
    int sizeof_itm=sizeof(unsigned char);
    int array_count=sizeof_arr/sizeof_itm;
    /*find the item in array */
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof_itm;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr;
    return *pos;
}

/*
 * Returns subtype of a previous spinning thing
 */
unsigned char get_spinningtng_prev(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_spinnthings;
    int sizeof_arr=sizeof(items_spinnthings);
    int sizeof_itm=sizeof(unsigned char);
    int array_count=sizeof_arr/sizeof_itm;
    /*find the item in array */
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof_itm;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr+(array_count-1)*sizeof_itm;
    return *pos;
}

/*
 * Returns true if the thing is a null thing
 */
short is_nulltng(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ITEM)
      return is_nulltng_stype(get_thing_subtype(thing));
    return false;
}

short is_nulltng_stype(const unsigned char stype_idx)
{
    if (get_nulltng_arridx(stype_idx)<0) return false;
    return true;
}

int get_nulltng_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_nullthings;
    int array_count=get_nulltng_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

/*
 * returns item on speficied index in the thing type array
 */
unsigned char get_nulltng_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_nullthings;
    int array_count=get_nulltng_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_nulltng_count(void)
{
     return sizeof(items_nullthings)/sizeof(unsigned char);
}

short is_false_stype(__attribute__((unused)) const unsigned char stype_idx)
{
    return false;
}

/*
 * Returns true if the thing is a item effect thing
 */
short is_itemeffect(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ITEM)
      return is_itemeffect_stype(get_thing_subtype(thing));
    return false;
}

short is_itemeffect_stype(const unsigned char stype_idx)
{
    if (get_itemeffect_arridx(stype_idx)<0) return false;
    return true;
}

int get_itemeffect_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_effcts;
    int array_count=get_itemeffect_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

/*
 * returns item on speficied index in the thing type array
 */
unsigned char get_itemeffect_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_effcts;
    int array_count=get_itemeffect_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_itemeffect_count(void)
{
     return sizeof(items_effcts)/sizeof(unsigned char);
}

/*
 * Returns true if the thing is a wrkshopbox thing
 */
short is_wrkshopbox(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ITEM)
      return is_wrkshopbox_stype(get_thing_subtype(thing));
    return false;
}

short is_wrkshopbox_stype(const unsigned char stype_idx)
{
    if (get_wrkshopbox_arridx(stype_idx)<0) return false;
    return true;
}

int get_wrkshopbox_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_wrkshpbxs;
    int array_count=get_wrkshopbox_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

/*
 * returns item on speficied index in the thing type array
 */
unsigned char get_wrkshopbox_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_wrkshpbxs;
    int array_count=get_wrkshopbox_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_wrkshopbox_count(void)
{
     return sizeof(items_wrkshpbxs)/sizeof(unsigned char);
}

/*
 * Returns true if the thing is a torch or candle
 */
short is_torchcndl(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ITEM)
      return is_torchcndl_stype(get_thing_subtype(thing));
    return false;
}

short is_torchcndl_stype(const unsigned char stype_idx)
{
    if (get_torchcndl_arridx(stype_idx)<0) return false;
    return true;
}

int get_torchcndl_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_torchcandls;
    int array_count=get_torchcndl_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

/*
 * returns item on speficied index in the thing type array
 */
unsigned char get_torchcndl_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_torchcandls;
    int array_count=get_torchcndl_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_torchcndl_count(void)
{
     return sizeof(items_torchcandls)/sizeof(unsigned char);
}

/*
 * Returns true if the thing is a roomequip thing
 */
short is_roomequip(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ITEM)
      return is_roomequip_stype(get_thing_subtype(thing));
    return false;
}

short is_roomequip_stype(const unsigned char stype_idx)
{
    if (get_roomequip_arridx(stype_idx)<0) return false;
    return true;
}

int get_roomequip_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_roomequip;
    int array_count=get_roomequip_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

/*
 * returns item on speficied index in the thing type array
 */
unsigned char get_roomequip_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_roomequip;
    int array_count=get_roomequip_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_roomequip_count(void)
{
     return sizeof(items_roomequip)/sizeof(unsigned char);
}

/*
 * Returns true if the thing is a pwhand thing
 */
short is_pwhand(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ITEM)
      return is_pwhand_stype(get_thing_subtype(thing));
    return false;
}

short is_pwhand_stype(const unsigned char stype_idx)
{
    if (get_pwhand_arridx(stype_idx)<0) return false;
    return true;
}

int get_pwhand_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_pwhand;
    int array_count=get_pwhand_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

/*
 * returns item on speficied index in the thing type array
 */
unsigned char get_pwhand_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_pwhand;
    int array_count=get_pwhand_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_pwhand_count(void)
{
     return sizeof(items_pwhand)/sizeof(unsigned char);
}

/*
 * Returns true if the thing is a dncrucial thing
 */
short is_dncrucial(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ITEM)
      return is_dncrucial_stype(get_thing_subtype(thing));
    return false;
}

short is_dncrucial_stype(const unsigned char stype_idx)
{
    if (get_dncrucial_arridx(stype_idx)<0) return false;
    return true;
}

int get_dncrucial_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_dncrucial;
    int array_count=get_dncrucial_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

/*
 * returns item on speficied index in the thing type array
 */
unsigned char get_dncrucial_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_dncrucial;
    int array_count=get_dncrucial_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_dncrucial_count(void)
{
     return sizeof(items_dncrucial)/sizeof(unsigned char);
}

/*
 * Returns if the thing is a furniture
 */
short is_furniture(const unsigned char *thing)
{
    /*All furniture are items */
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_furniture_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a furniture
 */
short is_furniture_stype(const unsigned char stype_idx)
{
    if (get_furniture_arridx(stype_idx)<0) return false;
    return true;
}

int get_furniture_arridx(const unsigned char stype_idx)
{
    unsigned char *arr=(unsigned char *)items_furniture;
    int array_count=get_furniture_count();
    /*searching for the subtype in array of this category */
    char *pos=memchr(arr,stype_idx,array_count);
    if (pos==NULL) return -1;
    return ((long)pos-(long)arr)/sizeof(unsigned char);
}

unsigned char get_furniture_arritm(const int arr_itm)
{
    unsigned char *arr=(unsigned char *)items_furniture;
    int array_count=get_furniture_count();
    if (arr_itm<0)
        return arr[0];
    if (arr_itm>=array_count)
        return arr[array_count-1];
    return arr[arr_itm];
}

unsigned int get_furniture_count(void)
{
     return sizeof(items_furniture)/sizeof(unsigned char);
}

/*
 * Returns subtype of a next furniture
 */
unsigned char get_furniture_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_furniture)/sizeof(unsigned char);
    /*find the furniture in items_furniture array */
    unsigned char *pos=memchr(items_furniture,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_furniture)||(pos>=items_furniture+sizeof(items_furniture)))
      pos=(unsigned char *)items_furniture;
    return *pos;
}

/*
 * Returns subtype of a previous furniture
 */
unsigned char get_furniture_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_furniture)/sizeof(unsigned char);
    /*find the furniture in items_furniture array */
    unsigned char *pos=memchr(items_furniture,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_furniture)||(pos>=items_furniture+sizeof(items_furniture)))
      pos=(unsigned char *)items_furniture+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

short is_herogate(const unsigned char *thing)
{
  if (get_thing_type(thing)==THING_TYPE_ITEM)
  if (get_thing_subtype(thing)==ITEM_SUBTYPE_HEROGATE)
        return true;
    return false;
}

short is_dnheart(const unsigned char *thing)
{
  if (get_thing_type(thing)==THING_TYPE_ITEM)
  if (get_thing_subtype(thing)==ITEM_SUBTYPE_DNHEART)
        return true;
    return false;
}

short is_doorkey(const unsigned char *thing)
{
  if (get_thing_type(thing)==THING_TYPE_ITEM)
  if (get_thing_subtype(thing)==ITEM_SUBTYPE_SPINNKEY)
        return true;
    return false;
}

short is_trainpost(const unsigned char *thing)
{
  if (get_thing_type(thing)==THING_TYPE_ITEM)
  if (get_thing_subtype(thing)==ITEM_SUBTYPE_TRAINPOST)
        return true;
    return false;
}

short is_room_inventory(const unsigned char *thing)
{
  switch (get_thing_type(thing))
  {
  case THING_TYPE_EFFECTGEN:
      switch (get_thing_subtype(thing))
      {
      case EFCTGEN_SUBTP_ENTRICE:  /* Entrance ice effect generator */
      case EFCTGEN_SUBTP_DRYICE:   /*Graveyard dry ice */
        return true;
      }
      return false;
  case THING_TYPE_DOOR:
      return true;
  case THING_TYPE_ITEM:
    { /* Some categories contain only room_things */
      int cat_idx=get_item_category(get_thing_subtype(thing));
      if ( (cat_idx==THING_CATEGR_ROOMEQUIP)||(cat_idx==THING_CATEGR_POLEBAR)
         ||(cat_idx==THING_CATEGR_TORCHCNDL)||(cat_idx==THING_CATEGR_DNCRUCIAL)
         ||(cat_idx==THING_CATEGR_HEARTFLAME) )
        return true;
      /* Also some other items which are in various categories */
      switch (get_thing_subtype(thing))
      {
      case ITEM_SUBTYPE_SPINNKEY:  /*Key marking closed doors */
      case ITEM_SUBTYPE_SPINNKEY2:
      case ITEM_SUBTYPE_TEMPLESTA:
      case ITEM_SUBTYPE_ANVIL:
        return true;
      }
    };return false;
  default:
    return false;
  }
}

/*
 * Returns true if the thing has affect on CLM entries
 * and should be passed to CLM generation routine
 */
short is_clmaffective_thing(const unsigned char *thing)
{
  switch (get_thing_type(thing))
  {
  case THING_TYPE_DOOR:
      return true;
  case THING_TYPE_ITEM:
    { /* Some categories contain only clmaffective things */
      int cat_idx=get_item_category(get_thing_subtype(thing));
      if ( (cat_idx==THING_CATEGR_ROOMEQUIP)
         ||(cat_idx==THING_CATEGR_TORCHCNDL)||(cat_idx==THING_CATEGR_DNCRUCIAL)
         ||(cat_idx==THING_CATEGR_HEARTFLAME) )
        return true;
      /* Also some other items which are in various categories */
      switch (get_thing_subtype(thing))
      {
      case ITEM_SUBTYPE_TEMPLESTA:
      case ITEM_SUBTYPE_ANVIL:
        return true;
      }
    };return false;
  default:
    return false;
  }
}

/*
 * Returns item type name as text
 */
char *get_item_subtype_fullname(const unsigned short stype_idx)
{
      switch (stype_idx)
      {
        case ITEM_SUBTYPE_NULL:
          return "Null";
        case ITEM_SUBTYPE_BARREL:
          return "Barrel";
        case ITEM_SUBTYPE_TORCH:
          return "Torch";
        case ITEM_SUBTYPE_GOLDCHEST:
          return "Gold Chest (500)";
        case ITEM_SUBTYPE_TEMPLESTA:
          return "Temple statue";
        case ITEM_SUBTYPE_DNHEART:
          return "Dungeon heart";
        case ITEM_SUBTYPE_GOLD:
          return "Gold Pot (250)";
        case ITEM_SUBTYPE_TORCHUN:
          return "Unlit torch";
        case ITEM_SUBTYPE_STATUEWO:
          return "Statue w/o flame";
        case ITEM_SUBTYPE_CHICKNGRW:
          return "Growing Chicken";
        case ITEM_SUBTYPE_CHICKN:
          return "Chicken";
        /*Spells */
        case ITEM_SUBTYPE_SPELLHOE:
          return "Hand of evil spell";
        case ITEM_SUBTYPE_SPELLIMP:
          return "Create imp spell";
        case ITEM_SUBTYPE_SPELLMUST:
          return "Must obey spell";
        case ITEM_SUBTYPE_SPELLSLAP:
          return "Slap spell";
        case ITEM_SUBTYPE_SPELLSOE:
          return "Sight of evil spel";
        case ITEM_SUBTYPE_SPELLCTA:
          return "Call to arms spell";
        case ITEM_SUBTYPE_SPELLCAVI:
          return "Cave in spell";
        case ITEM_SUBTYPE_SPELLHEAL:
          return "Heal creature spel";
        case ITEM_SUBTYPE_SPELLHLDA:
          return "Hold audience spel";
        case ITEM_SUBTYPE_SPELLLIGH:
          return "Lightning spell";
        case ITEM_SUBTYPE_SPELLSPDC:
          return "Speed creature spl";
        case ITEM_SUBTYPE_SPELLPROT:
          return "Protect creatr spl";
        case ITEM_SUBTYPE_SPELLCONC:
          return "Conceal creatr spl";

        case ITEM_SUBTYPE_NULL1:
          return "Null1";
        case ITEM_SUBTYPE_NULL2:
          return "Null2";

        case ITEM_SUBTYPE_ANVIL:
          return "Anvil";
        case ITEM_SUBTYPE_PRISONBAR:
          return "Prison bar";
        case ITEM_SUBTYPE_CANDLSTCK:
          return "Candlestick";
        case ITEM_SUBTYPE_GRAVSTONE:
          return "Gravestone";
        case ITEM_SUBTYPE_STATUHORN:
          return "Horny Statue";
        case ITEM_SUBTYPE_TRAINPOST:
          return "Training post";
        case ITEM_SUBTYPE_TORTSPIKE:
          return "Torture spike";
        case ITEM_SUBTYPE_TEMPLESPN:
          return "Temple Spangle";

        case ITEM_SUBTYPE_POTION1:
          return "Potion1";
        case ITEM_SUBTYPE_POTION2:
          return "Potion2";
        case ITEM_SUBTYPE_POTION3:
          return "Potion3";
        case ITEM_SUBTYPE_PWHAND:
          return "Power hand";
        case ITEM_SUBTYPE_PWHANDGRB:
          return "Power hand grab";
        case ITEM_SUBTYPE_PWHANDWHP:
          return "Power hand whip";
        case ITEM_SUBTYPE_CHICKNSTB:
          return "Chicken stable";
        case ITEM_SUBTYPE_CHICKNWOB:
          return "Chicken wobble";
        case ITEM_SUBTYPE_CHICKNCRK:
          return "Chicken crack";
        case ITEM_SUBTYPE_GOLDL:
          return "Gold (200)";
        case ITEM_SUBTYPE_SPINNKEY:
          return "Spinning key";
        /*Spells */
        case ITEM_SUBTYPE_SPELLDISE:
          return "Disease spell";
        case ITEM_SUBTYPE_SPELLCHKN:
          return "Chicken spell";
        case ITEM_SUBTYPE_SPELLDWAL:
          return "Destroy walls spel";
        case ITEM_SUBTYPE_SPELLTBMB:
          return "Time bomb spell";

        case ITEM_SUBTYPE_HEROGATE:
          return "Hero gate";
        case ITEM_SUBTYPE_SPINNKEY2: 
          return "Spinning key 2";

        case ITEM_SUBTYPE_ARMOUR: 
          return "Armour";
        case ITEM_SUBTYPE_GLDHOARD1: 
          return "Gold Hoarde 1";
        case ITEM_SUBTYPE_GLDHOARD2: 
          return "Gold Hoarde 2";
        case ITEM_SUBTYPE_GLDHOARD3: 
          return "Gold Hoarde 3";
        case ITEM_SUBTYPE_GLDHOARD4: 
          return "Gold Hoarde 4";
        case ITEM_SUBTYPE_GLDHOARD5: 
          return "Gold Hoarde 5";

        /*Creature lairs */
        case ITEM_SUBTYPE_LAIRWIZRD: 
          return "Wizard Lair";
        case ITEM_SUBTYPE_LAIRBARBR: 
          return "Barbarian Lair";
        case ITEM_SUBTYPE_LAIRARCHR: 
          return "Archer Lair";
        case ITEM_SUBTYPE_LAIRMONK: 
          return "Monk Lair";
        case ITEM_SUBTYPE_LAIRDWRFA: 
          return "Dwarfa Lair";
        case ITEM_SUBTYPE_LAIRKNGHT: 
          return "Knight Lair";
        case ITEM_SUBTYPE_LAIRAVATR: 
          return "Avatar Lair";
        case ITEM_SUBTYPE_LAIRTUNLR: 
          return "Tunneller Lair";
        case ITEM_SUBTYPE_LAIRWITCH: 
          return "Witch Lair";
        case ITEM_SUBTYPE_LAIRGIANT: 
          return "Giant Lair";
        case ITEM_SUBTYPE_LAIRFAIRY:
          return "Fairy Lair";
        case ITEM_SUBTYPE_LAIRTHEFT: 
          return "Thief Lair";
        case ITEM_SUBTYPE_LAIRSAMUR: 
          return "Samurai Lair";
        case ITEM_SUBTYPE_LAIRHORNY: 
          return "Horny Lair";
        case ITEM_SUBTYPE_LAIRSKELT: 
          return "Skeleton Lair";
        case ITEM_SUBTYPE_LAIRGOBLN: 
          return "Goblin Lair";
        case ITEM_SUBTYPE_LAIRDRAGN: 
          return "Dragon Lair";
        case ITEM_SUBTYPE_LAIRDEMSP: 
          return "Demonspawn Lair";
        case ITEM_SUBTYPE_LAIRFLY: 
          return "Fly Lair";
        case ITEM_SUBTYPE_LAIRDKMIS: 
          return "Dark Mistress Lair";
        case ITEM_SUBTYPE_LAIRSORCR: 
          return "Sorceror Lair";
        case ITEM_SUBTYPE_LAIRBILDM: 
          return "Bile Demon Lair";
        case ITEM_SUBTYPE_LAIRIMP: 
          return "Imp Lair";
        case ITEM_SUBTYPE_LAIRBUG: 
          return "Bug Lair";
        case ITEM_SUBTYPE_LAIRVAMP: 
          return "Vampire Lair";
        case ITEM_SUBTYPE_LAIRSPIDR: 
          return "Spider Lair";
        case ITEM_SUBTYPE_LAIRHLHND: 
          return "Hell Hound Lair";
        case ITEM_SUBTYPE_LAIRGHOST: 
          return "Ghost Lair";
        case ITEM_SUBTYPE_LAIRTENTC: 
          return "Tentacle Lair";

        /*Dungeon Specials */
        case ITEM_SUBTYPE_SPREVMAP: 
          return "Reveal map";
        case ITEM_SUBTYPE_SPRESURCT:
          return "Resurrect creatre";
        case ITEM_SUBTYPE_SPTRANSFR:
          return "Transfer creature";
        case ITEM_SUBTYPE_SPSTEALHR:
          return "Steal hero";
        case ITEM_SUBTYPE_SPMULTPLY:
          return "Multiply creatres";
        case ITEM_SUBTYPE_SPINCLEV:
          return "Increase level";
        case ITEM_SUBTYPE_SPMKSAFE:
          return "Make safe";
        case ITEM_SUBTYPE_SPHIDNWRL:
          return "Reveal hiddn world";
        case ITEM_SUBTYPE_SPCUSTOM:
          return "Custom box";
        /*Traps */
        case ITEM_SUBTYPE_TBBOULDER:
          return "Boulder trap box";
        case ITEM_SUBTYPE_TBALARM:
          return "Alarm trap box";
        case ITEM_SUBTYPE_TBPOISONG:
          return "Poison gas trapbx";
        case ITEM_SUBTYPE_TBLIGHTNG:
          return "Lightning trap bx";
        case ITEM_SUBTYPE_TBWRDOFPW:
          return "Word of Pwr trpbx";
        case ITEM_SUBTYPE_TBLAVA:
          return "Lava trap box";
        case ITEM_SUBTYPE_TBDUMMY2:
          return "Trap Dummy 2";
        case ITEM_SUBTYPE_TBDUMMY3:
          return "Trap Dummy 3";
        case ITEM_SUBTYPE_TBDUMMY4:
          return "Trap Dummy 4";
        case ITEM_SUBTYPE_TBDUMMY5:
          return "Trap Dummy 5";
        case ITEM_SUBTYPE_TBDUMMY6:
          return "Trap Dummy 6";
        case ITEM_SUBTYPE_TBDUMMY7:
          return "Trap Dummy 7";

        /*Workshop items */
        case ITEM_SUBTYPE_DBWOOD:
          return "Door Wood box";
        case ITEM_SUBTYPE_DBBRACE:
          return "Door Brace box";
        case ITEM_SUBTYPE_DBSTEEL:
          return "Door Steel box";
        case ITEM_SUBTYPE_DBMAGIC:
          return "Door Magic box";
        case ITEM_SUBTYPE_WBITEM:
          return "Workshop Item";
        /*Room equipment */
        case ITEM_SUBTYPE_HEARTFLMR:
          return "Heart flame(red)";
        case ITEM_SUBTYPE_DISEASE:
          return "Disease";
        case ITEM_SUBTYPE_SCAVNGEYE:
          return "Scavenger eye";
        case ITEM_SUBTYPE_WRKSHPMCH:
          return "Workshop machine";
        case ITEM_SUBTYPE_GURDFLAGR:
          return "Guard Flag(red)";
        case ITEM_SUBTYPE_GURDFLAGB:
          return "Guard Flag(blue)";
        case ITEM_SUBTYPE_GURDFLAGG:
          return "Guard Flag(green)";
        case ITEM_SUBTYPE_GURDFLAGY:
          return "Guard Flag(yellow)";
        case ITEM_SUBTYPE_FLAGPOST:
          return "Flagpost";
        case ITEM_SUBTYPE_HEARTFLMB:
          return "Heart flame(blue)";
        case ITEM_SUBTYPE_HEARTFLMG:
          return "Heart flame(green)";
        case ITEM_SUBTYPE_HEARTFLMY:
          return "Heart flame(yellw)";

        /*Special effects */
        case ITEM_SUBTYPE_PWSIGHT:
          return "Power Sight";
        case ITEM_SUBTYPE_PWLIGHTNG:
          return "Power Lightning";
        /*Other, mostly untested */
        case ITEM_SUBTYPE_TORTURER:
          return "Torturer";
        case ITEM_SUBTYPE_LAIRORC:
          return "Orc Lair";
        case ITEM_SUBTYPE_PWHANDGLD:
          return "Power Hand Gold";
        case ITEM_SUBTYPE_SPINNCOIN:
          return "Spinning Coin";

        case ITEM_SUBTYPE_STATUE2:
          return "Statue 2";
        case ITEM_SUBTYPE_STATUE3:
          return "Statue 3";
        case ITEM_SUBTYPE_STATUE4:
          return "Statue 4";
        case ITEM_SUBTYPE_STATUE5:
          return "Statue 5";
        case ITEM_SUBTYPE_SPELLARMG:
          return "Armageddon Spell";
        default:
          return "Unknown";
      }
}

/*
 * Returns object type name which is max 5 chars long
 */
char *get_item_subtype_shortname(const unsigned short stype_idx)
{
      switch (stype_idx)
      {
        case ITEM_SUBTYPE_NULL:
        case ITEM_SUBTYPE_NULL1:
        case ITEM_SUBTYPE_NULL2:
          return "Null";
        case ITEM_SUBTYPE_BARREL:
          return "Barrl";
        case ITEM_SUBTYPE_TORCH:
        case ITEM_SUBTYPE_TORCHUN:
          return "Torch";
        case ITEM_SUBTYPE_GOLDCHEST:
          return "GldCh";
        case ITEM_SUBTYPE_TEMPLESTA:
        case ITEM_SUBTYPE_STATUEWO:
        case ITEM_SUBTYPE_STATUHORN:
        case ITEM_SUBTYPE_STATUE2:
        case ITEM_SUBTYPE_STATUE3:
        case ITEM_SUBTYPE_STATUE4:
        case ITEM_SUBTYPE_STATUE5:
          return "Statu";
        case ITEM_SUBTYPE_DNHEART:
          return "Heart";
        case ITEM_SUBTYPE_GOLD:
        case ITEM_SUBTYPE_GOLDL:
          return "Gold";
        case ITEM_SUBTYPE_CHICKNGRW:
        case ITEM_SUBTYPE_CHICKN:
        case ITEM_SUBTYPE_CHICKNSTB:
        case ITEM_SUBTYPE_CHICKNWOB:
        case ITEM_SUBTYPE_CHICKNCRK:
          return "Food";
        /*Spells */
        case ITEM_SUBTYPE_SPELLHOE:
        case ITEM_SUBTYPE_SPELLIMP:
        case ITEM_SUBTYPE_SPELLMUST:
        case ITEM_SUBTYPE_SPELLSLAP:
        case ITEM_SUBTYPE_SPELLSOE:
        case ITEM_SUBTYPE_SPELLCTA:
        case ITEM_SUBTYPE_SPELLCAVI:
        case ITEM_SUBTYPE_SPELLHEAL:
        case ITEM_SUBTYPE_SPELLHLDA:
        case ITEM_SUBTYPE_SPELLLIGH:
        case ITEM_SUBTYPE_SPELLSPDC:
        case ITEM_SUBTYPE_SPELLPROT:
        case ITEM_SUBTYPE_SPELLCONC:
        case ITEM_SUBTYPE_SPELLDISE:
        case ITEM_SUBTYPE_SPELLCHKN:
        case ITEM_SUBTYPE_SPELLDWAL:
        case ITEM_SUBTYPE_SPELLTBMB:
        case ITEM_SUBTYPE_SPELLARMG:
          return "Spell";

        case ITEM_SUBTYPE_ANVIL:
          return "Anvil";
        case ITEM_SUBTYPE_PRISONBAR:
          return "PrBar";
        case ITEM_SUBTYPE_CANDLSTCK:
          return "Candl";
        case ITEM_SUBTYPE_GRAVSTONE:
          return "Grave";
        case ITEM_SUBTYPE_TRAINPOST:
          return "Trinr";
        case ITEM_SUBTYPE_TORTSPIKE:
          return "Spike";
        case ITEM_SUBTYPE_TEMPLESPN:
          return "Spngl";

        case ITEM_SUBTYPE_POTION1:
        case ITEM_SUBTYPE_POTION2:
        case ITEM_SUBTYPE_POTION3:
          return "Potin";
        case ITEM_SUBTYPE_PWHAND:
        case ITEM_SUBTYPE_PWHANDGRB:
        case ITEM_SUBTYPE_PWHANDWHP:
        case ITEM_SUBTYPE_PWHANDGLD:
          return "PHand";
        case ITEM_SUBTYPE_SPINNKEY:
        case ITEM_SUBTYPE_SPINNKEY2: 
          return "SpKey";

        case ITEM_SUBTYPE_HEROGATE:
          return "HGate";

        case ITEM_SUBTYPE_ARMOUR: 
          return "Armor";
        case ITEM_SUBTYPE_GLDHOARD1: 
        case ITEM_SUBTYPE_GLDHOARD2: 
        case ITEM_SUBTYPE_GLDHOARD3: 
        case ITEM_SUBTYPE_GLDHOARD4: 
        case ITEM_SUBTYPE_GLDHOARD5: 
          return "GldHr";
        /*Creature lairs */
        case ITEM_SUBTYPE_LAIRWIZRD: 
        case ITEM_SUBTYPE_LAIRBARBR: 
        case ITEM_SUBTYPE_LAIRARCHR: 
        case ITEM_SUBTYPE_LAIRMONK: 
        case ITEM_SUBTYPE_LAIRDWRFA: 
        case ITEM_SUBTYPE_LAIRKNGHT: 
        case ITEM_SUBTYPE_LAIRAVATR: 
        case ITEM_SUBTYPE_LAIRTUNLR: 
        case ITEM_SUBTYPE_LAIRWITCH: 
        case ITEM_SUBTYPE_LAIRGIANT: 
        case ITEM_SUBTYPE_LAIRFAIRY:
        case ITEM_SUBTYPE_LAIRTHEFT: 
        case ITEM_SUBTYPE_LAIRSAMUR: 
        case ITEM_SUBTYPE_LAIRHORNY: 
        case ITEM_SUBTYPE_LAIRSKELT: 
        case ITEM_SUBTYPE_LAIRGOBLN: 
        case ITEM_SUBTYPE_LAIRDRAGN: 
        case ITEM_SUBTYPE_LAIRDEMSP: 
        case ITEM_SUBTYPE_LAIRFLY: 
        case ITEM_SUBTYPE_LAIRDKMIS: 
        case ITEM_SUBTYPE_LAIRSORCR: 
        case ITEM_SUBTYPE_LAIRBILDM: 
        case ITEM_SUBTYPE_LAIRIMP: 
        case ITEM_SUBTYPE_LAIRBUG: 
        case ITEM_SUBTYPE_LAIRVAMP: 
        case ITEM_SUBTYPE_LAIRSPIDR: 
        case ITEM_SUBTYPE_LAIRHLHND: 
        case ITEM_SUBTYPE_LAIRGHOST: 
        case ITEM_SUBTYPE_LAIRTENTC: 
        case ITEM_SUBTYPE_LAIRORC:
          return "Lair";
        /*Dungeon Specials */
        case ITEM_SUBTYPE_SPREVMAP: 
        case ITEM_SUBTYPE_SPRESURCT:
        case ITEM_SUBTYPE_SPTRANSFR:
        case ITEM_SUBTYPE_SPSTEALHR:
        case ITEM_SUBTYPE_SPMULTPLY:
        case ITEM_SUBTYPE_SPINCLEV:
        case ITEM_SUBTYPE_SPMKSAFE:
        case ITEM_SUBTYPE_SPHIDNWRL:
        case ITEM_SUBTYPE_SPCUSTOM:
          return "SpcBx";
        /*Traps */
        case ITEM_SUBTYPE_TBBOULDER:
        case ITEM_SUBTYPE_TBALARM:
        case ITEM_SUBTYPE_TBPOISONG:
        case ITEM_SUBTYPE_TBLIGHTNG:
        case ITEM_SUBTYPE_TBWRDOFPW:
        case ITEM_SUBTYPE_TBLAVA:
        case ITEM_SUBTYPE_TBDUMMY2:
        case ITEM_SUBTYPE_TBDUMMY3:
        case ITEM_SUBTYPE_TBDUMMY4:
        case ITEM_SUBTYPE_TBDUMMY5:
        case ITEM_SUBTYPE_TBDUMMY6:
        case ITEM_SUBTYPE_TBDUMMY7:
          return "TrpBx";
        /*Workshop items */
        case ITEM_SUBTYPE_DBWOOD:
        case ITEM_SUBTYPE_DBBRACE:
        case ITEM_SUBTYPE_DBSTEEL:
        case ITEM_SUBTYPE_DBMAGIC:
          return "DorBx";
        case ITEM_SUBTYPE_WBITEM:
          return "WshBx";
        /*Room equipment */
        case ITEM_SUBTYPE_HEARTFLMR:
        case ITEM_SUBTYPE_HEARTFLMB:
        case ITEM_SUBTYPE_HEARTFLMG:
        case ITEM_SUBTYPE_HEARTFLMY:
          return "Flame";
        case ITEM_SUBTYPE_DISEASE:
          return "Dises";
        case ITEM_SUBTYPE_SCAVNGEYE:
          return "ScEye";
        case ITEM_SUBTYPE_WRKSHPMCH:
          return "Machn";
        case ITEM_SUBTYPE_GURDFLAGR:
        case ITEM_SUBTYPE_GURDFLAGB:
        case ITEM_SUBTYPE_GURDFLAGG:
        case ITEM_SUBTYPE_GURDFLAGY:
          return "Flag";
        case ITEM_SUBTYPE_FLAGPOST:
          return "FlagP";
        case ITEM_SUBTYPE_PWSIGHT:
          return "PwSig";
        case ITEM_SUBTYPE_PWLIGHTNG:
          return "PwLgh";
        case ITEM_SUBTYPE_TORTURER:
          return "Tortr";
        case ITEM_SUBTYPE_SPINNCOIN:
          return "Coin";
        default:
          return "thing";
      }
}

/*
 * Returns itam category for given subtype.
 * Note: works only for items, not all things!
 */
int get_item_category(unsigned char stype_idx)
{
    int func_count=sizeof(item_subtype_tests)/sizeof(is_item_subtype);
    int i;
    for (i=0;i<func_count;i++)
    {
      if (item_subtype_tests[i](stype_idx))
        return i;
    }
    return THING_CATEGR_UNKNOWN;
}

/*
 * Returns creature type name string
 */
char *get_creature_subtype_fullname(const unsigned short stype_idx)
{
     int types_count=sizeof(creature_fullnames)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)creature_fullnames[stype_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns creature type name which is max 5 chars long
 */
char *get_creature_subtype_shortname(const unsigned short stype_idx)
{
     int types_count=sizeof(creature_names)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)creature_names[stype_idx];
     else
       return "unknw";
}

/*
 * Returns trap type name string
 */
char *get_trap_subtype_fullname(const unsigned short stype_idx)
{
     int types_count=sizeof(traps)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)traps[stype_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns door type name string
 */
char *get_door_subtype_fullname(const unsigned short stype_idx)
{
     int types_count=sizeof(doors)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)doors[stype_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns thing category text for given subtype
 */
char *get_thing_category_fullname(const unsigned short arr_idx)
{
     int types_count=sizeof(thing_subtype_fullnames)/sizeof(char *);
     if (arr_idx<types_count)
       return (char *)thing_subtype_fullnames[arr_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns short thing category text for given subtype
 */
char *get_thing_category_shortname(const unsigned short arr_idx)
{
     int types_count=sizeof(thing_subtype_shortnames)/sizeof(char *);
     if (arr_idx<types_count)
       return (char *)thing_subtype_shortnames[arr_idx];
     else
       return "unknwn(?)";
}

/*
 * Returns effect generator description string
 */
char *get_effectgen_subtype_fullname(const unsigned short stype_idx)
{
     int types_count=sizeof(effectgenrts)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)effectgenrts[stype_idx];
     else
       return "unknown(?!)";
}

short item_verify(unsigned char *thing, struct VERIFY_OPTIONS *verif_opt)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  switch (get_item_category(stype_idx))
  {
    case THING_CATEGR_EFFCTGEN:
    case THING_CATEGR_ITEMEFFCT:
    case THING_CATEGR_CREATR:
    case THING_CATEGR_CREATLAIR:
    case THING_CATEGR_TRAP:
    case THING_CATEGR_DOOR:
    case THING_CATEGR_SPECIALBOX:
    case THING_CATEGR_SPELLBOOK:
    case THING_CATEGR_WRKSHOPBOX:
    case THING_CATEGR_SPINNTNG:
    case THING_CATEGR_FOOD:
    case THING_CATEGR_GOLD:
    case THING_CATEGR_TORCHCNDL:
    case THING_CATEGR_HEARTFLAME:
    case THING_CATEGR_POLEBAR:
    case THING_CATEGR_STATUE:
    case THING_CATEGR_FURNITURE:
    case THING_CATEGR_ROOMEQUIP:
    case THING_CATEGR_PWHAND:
    case THING_CATEGR_DNCRUCIAL:
      break;
    case THING_CATEGR_NULL:
      sprintf(verif_opt->err_msg,"Null item subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
    case THING_CATEGR_UNKNOWN:
    default:
      sprintf(verif_opt->err_msg,"Unknown item subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
  }
  int sen_tl;
  sen_tl=get_thing_sensitile(thing);
  int sx,sy;
  sx=get_thing_subtile_x(thing);
  sy=get_thing_subtile_y(thing);
  int tx,ty;
  tx=sx/MAP_SUBNUM_X;
  ty=sy/MAP_SUBNUM_Y;
  if (sen_tl!=THING_SENSITILE_NONE)
  {
      /* Generate warning if sensitile coordinates are wrong. */
      /* Ignore such error for items which usually have it wrongly set. */
      int sen_tlx,sen_tly;
      sen_tlx=sen_tl%verif_opt->tlsize.x;
      sen_tly=sen_tl/verif_opt->tlsize.x;
      if (((tx<sen_tlx-1)||(tx>sen_tlx+1)||
           (ty<sen_tly-1)||(ty>sen_tly+1)) &&
           (stype_idx!=ITEM_SUBTYPE_SPINNKEY))
      {
          sprintf(verif_opt->err_msg,"Item has wrong sensitive tile number");
          return VERIF_WARN;
      }
  }
  return VERIF_OK;
}

short creature_verify(unsigned char *thing, struct VERIFY_OPTIONS *verif_opt)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  if ((stype_idx>CREATR_SUBTP_FLOAT)||(stype_idx<CREATR_SUBTP_WIZRD))
  {
      sprintf(verif_opt->err_msg,"Unknown creature subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
  }
  return VERIF_OK;
}

short effectgen_verify(unsigned char *thing, struct VERIFY_OPTIONS *verif_opt)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  if ((stype_idx>EFCTGEN_SUBTP_DRYICE)||(stype_idx<EFCTGEN_SUBTP_LAVA))
  {
      sprintf(verif_opt->err_msg,"Unknown effect generator subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
  }
  int sen_tl;
  sen_tl=get_thing_sensitile(thing);
  int sx,sy;
  sx=get_thing_subtile_x(thing);
  sy=get_thing_subtile_y(thing);
  int tx,ty;
  tx=sx/MAP_SUBNUM_X;
  ty=sy/MAP_SUBNUM_Y;
  if (sen_tl!=THING_SENSITILE_NONE)
  {
      /* Generate warning if sensitile coordinates are wrong. */
      /* Ignore such error for effects which usually have it wrongly set. */
      int sen_tlx,sen_tly;
      sen_tlx=sen_tl%verif_opt->tlsize.x;
      sen_tly=sen_tl/verif_opt->tlsize.x;
      if (((tx<sen_tlx-1)||(tx>sen_tlx+1)||
           (ty<sen_tly-1)||(ty>sen_tly+1)) &&
           (stype_idx!=EFCTGEN_SUBTP_ENTRICE))
      {
          sprintf(verif_opt->err_msg,"Effect generator has wrong sensitive tile number");
          return VERIF_WARN;
      }
  }
  int range_stl;
  range_stl=get_thing_range_subtile(thing);
  if (range_stl>MAP_SIZE_DKSTD_X)
  {
      sprintf(verif_opt->err_msg,"Effect generator range exceeds %d subtiles",MAP_SIZE_DKSTD_X);
      return VERIF_WARN;
  }
  return VERIF_OK;
}

short door_verify(unsigned char *thing, struct VERIFY_OPTIONS *verif_opt)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  if ((stype_idx>DOOR_SUBTYPE_MAGIC)||(stype_idx<DOOR_SUBTYPE_WOOD))
  {
      sprintf(verif_opt->err_msg,"Unknown door subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
  }
  return VERIF_OK;
}

short trap_verify(unsigned char *thing, struct VERIFY_OPTIONS *verif_opt)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  if ((stype_idx>TRAP_SUBTYPE_LAVA)||(stype_idx<TRAP_SUBTYPE_BOULDER))
  {
      sprintf(verif_opt->err_msg,"Unknown trap subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
  }
  return VERIF_OK;
}

short thing_verify(unsigned char *thing, struct VERIFY_OPTIONS *verif_opt)
{
  int pos_h=get_thing_subtile_h(thing);
  int pos_x=get_thing_subtile_x(thing);
  int pos_y=get_thing_subtile_y(thing);
  if ((pos_h>=MAP_SUBNUM_H) || (pos_x>=verif_opt->tlsize.x*MAP_SUBNUM_X) ||
      (pos_y>=verif_opt->tlsize.y*MAP_SUBNUM_Y))
  {
    sprintf(verif_opt->err_msg,"Thing position (%d,%d,%d) invalid",pos_x,pos_y,pos_h);
    return VERIF_WARN;
  }
  switch(get_thing_type(thing))
  {
    case THING_TYPE_ITEM:
      return item_verify(thing, verif_opt);
    case THING_TYPE_CREATURE:
      return creature_verify(thing, verif_opt);
    case THING_TYPE_EFFECTGEN:
      return effectgen_verify(thing, verif_opt);
    case THING_TYPE_TRAP:
      return trap_verify(thing, verif_opt);
    case THING_TYPE_DOOR:
      return door_verify(thing, verif_opt);
    default:
      sprintf(verif_opt->err_msg,"Unknown thing type (%d)",(int)get_thing_type(thing));
      return VERIF_WARN;
  }
}

/*
 * Returns TRUE if subtile with given coordinates is within range of
 * given thing.
 */
short subtl_in_thing_range(const unsigned char *thing,unsigned int sx,unsigned int sy)
{
  if (thing==NULL) return false;
  float pos_x=(float)get_thing_subtile_x(thing);
  float pos_y=(float)get_thing_subtile_y(thing);
  unsigned int rng=get_thing_range_adv(thing);
  float distance=sqrt(pow(pos_x-(float)sx,2)+pow(pos_y-(float)sy,2))*256.0;
  if (distance<=rng) return true;
  return false;
}
