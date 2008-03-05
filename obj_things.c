/*
 * obj_things.c
 *
 * Defines routines for maintaining single TNG entry - a thing
 *
 */

#include "obj_things.h"

#include <math.h>
#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"
#include "lev_things.h"
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
0x0c thing_sensitile
0x0d door_orientation
0x0e thing_level
0x0f
0x10
0x11
0x12
0x13
0x14
*/

const char *thing_fullnames[]={"Nothing", "Item/decoration", "unknown 2",
    "unknown 3", "unknown 4", "Creature", "unknown 6", "Room effect",
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
const char *roomeffects[]={"unknown 0", "Lava", "Dripping water",
    "Rock Fall", "Entrance Ice", "Dry ice", "unknown 6", "unknown 7",
    "unknown 8"};

const unsigned char items_specboxes[]={
      ITEM_SUBTYPE_SPREVMAP,ITEM_SUBTYPE_SPRESURCT,ITEM_SUBTYPE_SPTRANSFR,
      ITEM_SUBTYPE_SPSTEALHR,ITEM_SUBTYPE_SPMULTPLY,ITEM_SUBTYPE_SPINCLEV,
      ITEM_SUBTYPE_SPMKSAFE,ITEM_SUBTYPE_SPHIDNWRL};

const unsigned char items_spellbooks[]={
      ITEM_SUBTYPE_SPELLHOE,ITEM_SUBTYPE_SPELLIMP,ITEM_SUBTYPE_SPELLMUST,
      ITEM_SUBTYPE_SPELLSLAP,ITEM_SUBTYPE_SPELLSOE,ITEM_SUBTYPE_SPELLCTA,
      ITEM_SUBTYPE_SPELLCAVI,ITEM_SUBTYPE_SPELLHEAL,ITEM_SUBTYPE_SPELLHLDA,
      ITEM_SUBTYPE_SPELLLIGH,ITEM_SUBTYPE_SPELLSPDC,ITEM_SUBTYPE_SPELLPROT,
      ITEM_SUBTYPE_SPELLCONC,ITEM_SUBTYPE_SPELLDISE,ITEM_SUBTYPE_SPELLCHKN,
      ITEM_SUBTYPE_SPELLDWAL,ITEM_SUBTYPE_SPELLTBMB,ITEM_SUBTYPE_SPELLARMG};

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

const unsigned char items_trapbxs[]={
      ITEM_SUBTYPE_TBBOULDER,ITEM_SUBTYPE_TBALARM,ITEM_SUBTYPE_TBPOISONG,
      ITEM_SUBTYPE_TBLIGHTNG,ITEM_SUBTYPE_TBWRDOFPW,ITEM_SUBTYPE_TBLAVA,
      ITEM_SUBTYPE_TBDUMMY2,ITEM_SUBTYPE_TBDUMMY3,ITEM_SUBTYPE_TBDUMMY4,
      ITEM_SUBTYPE_TBDUMMY5,ITEM_SUBTYPE_TBDUMMY6,ITEM_SUBTYPE_TBDUMMY7};

const unsigned char items_traps[]={
      TRAP_SUBTYPE_BOULDER,TRAP_SUBTYPE_ALARM,TRAP_SUBTYPE_GAS,
      TRAP_SUBTYPE_LIGHTNG,TRAP_SUBTYPE_WORDPWR,TRAP_SUBTYPE_LAVA,
      TRAP_SUBTYPE_DUMMY2,TRAP_SUBTYPE_DUMMY3,TRAP_SUBTYPE_DUMMY4,
      TRAP_SUBTYPE_DUMMY5,TRAP_SUBTYPE_DUMMY6,TRAP_SUBTYPE_DUMMY7};

const unsigned char items_doors[]={
      ITEM_SUBTYPE_DBWOOD,ITEM_SUBTYPE_DBBRACE,
      ITEM_SUBTYPE_DBSTEEL,ITEM_SUBTYPE_DBMAGIC};

const unsigned char items_statues[]={
      ITEM_SUBTYPE_STATUEWO,ITEM_SUBTYPE_STATUHORN,ITEM_SUBTYPE_STATUE2,
      ITEM_SUBTYPE_STATUE3,ITEM_SUBTYPE_STATUE4,ITEM_SUBTYPE_STATUE5,
      ITEM_SUBTYPE_STATUE6,ITEM_SUBTYPE_TEMPLESTA};

const unsigned char items_furniture[]={
      ITEM_SUBTYPE_BARREL,ITEM_SUBTYPE_POTION1,ITEM_SUBTYPE_POTION2,
      ITEM_SUBTYPE_POTION3,ITEM_SUBTYPE_ARMOUR,ITEM_SUBTYPE_CANDLSTCK };

const unsigned char items_gold[]={
      ITEM_SUBTYPE_GOLDCHEST,ITEM_SUBTYPE_GOLD,ITEM_SUBTYPE_COIN,
      ITEM_SUBTYPE_GLDHOARD1,ITEM_SUBTYPE_GLDHOARD2,ITEM_SUBTYPE_GLDHOARD3,
      ITEM_SUBTYPE_GLDHOARD4,ITEM_SUBTYPE_GLDHOARD5, };

const unsigned char items_food[]={
      ITEM_SUBTYPE_CHICKNGRW, ITEM_SUBTYPE_CHICKN, ITEM_SUBTYPE_CHICKNSTB,
      ITEM_SUBTYPE_CHICKNWOB, ITEM_SUBTYPE_CHICKNCRK, ITEM_SUBTYPE_CHICKN2, };

const unsigned char items_torches[]={
      ITEM_SUBTYPE_TORCHUN, ITEM_SUBTYPE_TORCH, };

const unsigned char items_heartflames[]={
      ITEM_SUBTYPE_HEARTFLMR, ITEM_SUBTYPE_HEARTFLMB, ITEM_SUBTYPE_HEARTFLMG,
      ITEM_SUBTYPE_HEARTFLMY,};

const unsigned char items_litthings[]={
      ITEM_SUBTYPE_TORCH, ITEM_SUBTYPE_CANDLSTCK,};

const unsigned char creatr_types[]={
      CREATR_SUBTP_WIZRD,CREATR_SUBTP_BARBARIN,CREATR_SUBTP_ARCHER,
      CREATR_SUBTP_MONK,CREATR_SUBTP_DWAFT,CREATR_SUBTP_KNIGHT,
      CREATR_SUBTP_AVATAR,CREATR_SUBTP_TUNELER,CREATR_SUBTP_WITCH,
      CREATR_SUBTP_GIANT,CREATR_SUBTP_FAIRY,CREATR_SUBTP_THEFT,
      CREATR_SUBTP_SMURI,CREATR_SUBTP_REAPER,CREATR_SUBTP_SKELETON,
      CREATR_SUBTP_TROLL,CREATR_SUBTP_DRAGON,CREATR_SUBTP_SPAWN,
      CREATR_SUBTP_FLY,CREATR_SUBTP_MISTRESS,CREATR_SUBTP_WARLOCK,
      CREATR_SUBTP_BILEDEMN,CREATR_SUBTP_IMP,CREATR_SUBTP_BEETLE,
      CREATR_SUBTP_VAMPIRE,CREATR_SUBTP_SPIDER,CREATR_SUBTP_HOUND,
      CREATR_SUBTP_GHOST,CREATR_SUBTP_TENTCL,CREATR_SUBTP_ORC,
      CREATR_SUBTP_FLOAT};

const unsigned char door_types[]={
      DOOR_SUBTYPE_WOOD,DOOR_SUBTYPE_BRACED,
      DOOR_SUBTYPE_IRON,DOOR_SUBTYPE_MAGIC};

const unsigned char roomefct_types[]={
      ROOMEFC_SUBTP_LAVA,ROOMEFC_SUBTP_DRIPWTR,ROOMEFC_SUBTP_ROCKFAL,
      ROOMEFC_SUBTP_ENTRICE,ROOMEFC_SUBTP_DRYICE
      };

const is_thing_subtype thing_subtype_tests[]={
      is_spellbook, is_dngspecbox, is_crtrlair,
      is_trapbox,   is_trap,       is_creature,
      is_door,      is_roomeffect, is_statue,
      is_food,      is_gold,       is_torch,
      is_heartflame,is_furniture,
      };

const thing_subtype_switch thing_subtype_next[]={
      get_spellbook_next, get_dngspecbox_next, get_crtrlair_next,
      get_trapbox_next,   get_trap_next,       get_creature_next,
      get_door_next,      get_roomeffect_next, get_statue_next,
      get_food_next,      get_gold_next,       get_torch_next,
      get_heartflame_next,get_furniture_next,
      };

const thing_subtype_switch thing_subtype_prev[]={
      get_spellbook_prev, get_dngspecbox_prev, get_crtrlair_prev,
      get_trapbox_prev,   get_trap_prev,       get_creature_prev,
      get_door_prev,      get_roomeffect_prev, get_statue_prev,
      get_food_prev,      get_gold_prev,       get_torch_prev,
      get_heartflame_prev,get_furniture_prev,
      };

// True means TNG/APT/LGT are updated automatically
short obj_auto_update=true;

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

short set_thing_subtype(unsigned char *thing,unsigned char stype_idx)
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
    if (thing==NULL) return 0;
    return (unsigned char)thing[1];
}

short set_thing_subtile_x(unsigned char *thing,unsigned char pos_x)
{
    if (thing==NULL) return 0;
    thing[1]=pos_x;
}

unsigned char get_thing_subtile_y(const unsigned char *thing)
{
    if (thing==NULL) return 0;
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
    return (char)thing[0];
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
 * Returns range for any thing, as subtiles*256+within_subtile
 */
unsigned int get_thing_range_adv(const unsigned char *thing)
{
  switch(get_thing_type(thing))
  {
    case THING_TYPE_ITEM:
      return (1<<8);
    case THING_TYPE_CREATURE:
      return (4<<8);
    case THING_TYPE_ROOMEFFECT:
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

short switch_thing_subtype(unsigned char *thing,short forward)
{
    if (thing==NULL) return false;
    int func_count=sizeof(thing_subtype_tests)/sizeof(is_thing_subtype);
    int i;
    for (i=0;i<func_count;i++)
      if (thing_subtype_tests[i](thing))
      {
        unsigned char stype_idx=get_thing_subtype(thing);
        unsigned char stype_new;
        if (forward)
          stype_new=thing_subtype_next[i](stype_idx);
        else
          stype_new=thing_subtype_prev[i](stype_idx);
        if (stype_new!=stype_idx)
        {
          set_thing_subtype(thing,stype_new);
          return true;
        }
        return false;
      }
    return false;
}

char *get_thing_type_fullname(unsigned short type_idx)
{
     int types_count=sizeof(thing_fullnames)/sizeof(char *);
     if (type_idx<types_count)
       return (char *)thing_fullnames[type_idx];
     else
       return "unknown(?!)";
}

char *get_thing_type_shortname(unsigned short type_idx)
{
     int types_count=sizeof(thing_shortnames)/sizeof(char *);
     if (type_idx<types_count)
       return (char *)thing_shortnames[type_idx];
     else
       return "unkn!";
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
  case THING_TYPE_ROOMEFFECT:
      {
        switch (stype_idx)
        {
        case ROOMEFC_SUBTP_LAVA:
            return SLAB_TYPE_LAVA;
        case ROOMEFC_SUBTP_DRIPWTR:
            //return SLAB_TYPE_WATER;
            //Changed because this is sometimes placed as "user-made effect"
            // (usually on lava)
            return SLAB_TYPE_CLAIMED;
        case ROOMEFC_SUBTP_ROCKFAL:
            return SLAB_TYPE_PATH;
        case ROOMEFC_SUBTP_ENTRICE:
            //return SLAB_TYPE_PORTAL;
            //Changed because this is sometimes placed as "user-made effect"
            // (usually on hero gates)
            return SLAB_TYPE_CLAIMED;
        case ROOMEFC_SUBTP_DRYICE:
            //return SLAB_TYPE_GRAVEYARD;
            //Changed because this is sometimes placed as "user-made effect"
            //(on dyngeon specials and in other places)
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
  case ITEM_CATEGR_FOOD:
      return SLAB_TYPE_HATCHERY;
  case ITEM_CATEGR_CREATLAIR:
      return SLAB_TYPE_LAIR;
  case ITEM_CATEGR_LIGHTS:
      return SLAB_TYPE_CLAIMED;
  case ITEM_CATEGR_ROOMEQUIP:
    {
    //TODO: validate when all rooms will be done
        switch (stype_idx)
        {
        case ITEM_SUBTYPE_DNHEART:
        case ITEM_SUBTYPE_HEARTFLMR:
        case ITEM_SUBTYPE_HEARTFLMB:
        case ITEM_SUBTYPE_HEARTFLMG:
        case ITEM_SUBTYPE_HEARTFLMY:
          return SLAB_TYPE_DUNGHEART;
        case ITEM_SUBTYPE_FLAGPOST:
        case ITEM_SUBTYPE_GURDFLAGR:
        case ITEM_SUBTYPE_GURDFLAGB:
        case ITEM_SUBTYPE_GURDFLAGG:
        case ITEM_SUBTYPE_GURDFLAGY:
          return SLAB_TYPE_GUARDPOST;
        //TODO: not sure about these
        case ITEM_SUBTYPE_CANDLSTCK:
        case ITEM_SUBTYPE_BARREL:
        case ITEM_SUBTYPE_HEROGATE:
          return SLAB_TYPE_CLAIMED;
        case ITEM_SUBTYPE_TEMPLESPN:
          return SLAB_TYPE_TEMPLE;
        case ITEM_SUBTYPE_TEMPLESTA:
          //Part of a temple, but also used in hero teritory
          return SLAB_TYPE_CLAIMED;
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
unsigned char *create_thing_empty()
{
    unsigned char *thing;
    thing = (unsigned char *)malloc(SIZEOF_DK_TNG_REC);
    if (thing==NULL)
      die ("create_thing: Out of memory.");
    return thing;
}

/*
 * Creates a new thing, prepared to be placed in given coordinates,
 * but without placing it into LEVEL structure
 */
unsigned char *create_thing(unsigned int sx, unsigned int sy)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    sx%=arr_entries_x;
    sy%=arr_entries_y;

    unsigned char *thing;
    thing = create_thing_empty();
    int i;
    for (i=0; i < SIZEOF_DK_TNG_REC; i++)
      thing[i]=0;
    set_thing_subtpos(thing,((sx%MAP_SUBNUM_X)*0x40+0x40),((sy%MAP_SUBNUM_Y)*0x40+0x40));
    set_thing_subtile(thing,(unsigned char)sx,(unsigned char)sy);
    set_thing_subtile_h(thing,1); // Default height is floor height
    // Setting the owner
    set_thing_owner(thing,PLAYER_UNSET);
    return thing;
}

/*
 * Creates a new thing, prepared to be placed in given coordinates,
 * with body copied from another thing, given as parameter.
 */
unsigned char *create_thing_copy(unsigned int sx, unsigned int sy,unsigned char *src)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    sx%=arr_entries_x;
    sy%=arr_entries_y;

    unsigned char *thing;
    thing = create_thing_empty();
    int i;
    memcpy(thing,src,SIZEOF_DK_TNG_REC);
    unsigned char type_idx=get_thing_type(thing);
    unsigned char stype_idx=get_thing_subtype(thing);
    set_thing_subtpos(thing,((sx%MAP_SUBNUM_X)*0x40+0x40),((sy%MAP_SUBNUM_Y)*0x40+0x40));
    set_thing_subtile(thing,(unsigned char)sx,(unsigned char)sy);
    if (type_idx==THING_TYPE_ROOMEFFECT)
      set_thing_sensitile(thing,compute_roomeffect_sensitile(lvl,thing));
    if (type_idx==THING_TYPE_ITEM)
      set_thing_sensitile(thing,compute_item_sensitile(lvl,thing));
    return thing;
}

/*
 * Creates a new thing of type item
 */
unsigned char *create_item(unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    set_thing_type(thing,THING_TYPE_ITEM);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,tx,ty));
    set_thing_sensitile(thing,THING_SENSITILE_NONE); //Default associated tile is none
    return thing;
}

/*
 * Returns if the thing is a spellbook
 */
short is_spellbook(const unsigned char *thing)
{
    //All spells are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_spellbook_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing is a spellbook
 */
short is_spellbook_stype(const unsigned char stype_idx)
{
     int array_count=sizeof(items_spellbooks)/sizeof(unsigned char);
    //these are listed in items_spellbooks array
    char *pos=memchr(items_spellbooks,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
}

/*
 * Returns subtype of a next spellbook
 */
unsigned char get_spellbook_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_spellbooks)/sizeof(unsigned char);
    //find the spell in items_spellbooks array
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
    //find the spell in items_spellbooks array
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
    //All specials are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_dngspecbox_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a dungeon special box
 */
short is_dngspecbox_stype(const unsigned char stype_idx)
{
     int array_count=sizeof(items_specboxes)/sizeof(unsigned char);
    //these are listed in items_specboxes array
    char *pos=memchr(items_specboxes,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
}

/*
 * Returns subtype of a next dungeon special box
 */
unsigned char get_dngspecbox_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_specboxes)/sizeof(unsigned char);
    //find the special in items_specboxes array
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
    //find the special in items_specboxes array
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
    //All lairs are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_crtrlair_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a creature lair
 */
short is_crtrlair_stype(const unsigned char stype_idx)
{
     int array_count=sizeof(items_crtrlairs)/sizeof(unsigned char);
    //these are listed in items_crtrlairs array
    char *pos=memchr(items_crtrlairs,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
}

/*
 * Returns subtype of a next creature lair
 */
unsigned char get_crtrlair_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_crtrlairs)/sizeof(unsigned char);
    //find the lair in items_crtrlairs array
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
    //find the lair in items_crtrlairs array
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
    //All traps are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_trapbox_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a trap box
 */
short is_trapbox_stype(const unsigned char stype_idx)
{
     int array_count=sizeof(items_trapbxs)/sizeof(unsigned char);
    //these are listed in items_trapbxs array
    char *pos=memchr(items_trapbxs,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
}

/*
 * Returns subtype of a next trap box
 */
unsigned char get_trapbox_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_trapbxs)/sizeof(unsigned char);
    //find the trap box in items_trapbxs array
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
    //find the trap box in items_trapbxs array
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

/*
 * Returns subtype of a next deployed trap
 */
unsigned char get_trap_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_traps)/sizeof(unsigned char);
    //find the trap in items_traps array
    unsigned char *pos=memchr(items_traps,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<items_traps)||(pos>=items_traps+sizeof(items_traps)))
      pos=(unsigned char *)items_traps;
    return *pos;
}

/*
 * Returns subtype of a previous deployed trap
 */
unsigned char get_trap_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(items_traps)/sizeof(unsigned char);
    //find the trap in items_traps array
    unsigned char *pos=memchr(items_traps,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_traps)||(pos>=items_traps+sizeof(items_traps)))
      pos=(unsigned char *)items_traps+(array_count-1)*sizeof(unsigned char);
    return *pos;
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

/*
 * Returns subtype of a next creature kind
 */
unsigned char get_creature_next(const unsigned char stype_idx)
{
     int array_count=sizeof(creatr_types)/sizeof(unsigned char);
    //find the creature in creatr_types array
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
    //find the creature in creatr_types array
    unsigned char *pos=memchr(creatr_types,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<creatr_types)||(pos>=creatr_types+sizeof(creatr_types)))
      pos=(unsigned char *)creatr_types+(array_count-1)*sizeof(unsigned char);
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

/*
 * Returns subtype of a next door type
 */
unsigned char get_door_next(const unsigned char stype_idx)
{
     int array_count=sizeof(door_types)/sizeof(unsigned char);
    //find the door in door_types array
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
    //find the door in door_types array
    unsigned char *pos=memchr(door_types,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<door_types)||(pos>=door_types+sizeof(door_types)))
      pos=(unsigned char *)door_types+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a room effect
 */
short is_roomeffect(const unsigned char *thing)
{
    if (get_thing_type(thing) == THING_TYPE_ROOMEFFECT)
      return true;
    return false;
}

/*
 * Returns subtype of a next room effect type
 */
unsigned char get_roomeffect_next(const unsigned char stype_idx)
{
     int array_count=sizeof(roomefct_types)/sizeof(unsigned char);
    //find the effect in roomefct_types array
    unsigned char *pos=memchr(roomefct_types,stype_idx,array_count);
    if (pos!=NULL) pos+=sizeof(unsigned char);
    if ((pos<roomefct_types)||(pos>=roomefct_types+sizeof(roomefct_types)))
      pos=(unsigned char *)roomefct_types;
    return *pos;
}

/*
 * Returns subtype of a previous room effect type
 */
unsigned char get_roomeffect_prev(const unsigned char stype_idx)
{
     int array_count=sizeof(roomefct_types)/sizeof(unsigned char);
    //find the effect in roomefct_types array
    unsigned char *pos=memchr(roomefct_types,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<roomefct_types)||(pos>=roomefct_types+sizeof(roomefct_types)))
      pos=(unsigned char *)roomefct_types+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a statue
 */
short is_statue(const unsigned char *thing)
{
    //All statues are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_statue_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a statue
 */
short is_statue_stype(const unsigned char stype_idx)
{
     int array_count=sizeof(items_statues)/sizeof(unsigned char);
    //these are listed in items_statues array
    char *pos=memchr(items_statues,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
}

/*
 * Returns subtype of a next statue
 */
unsigned char get_statue_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_statues)/sizeof(unsigned char);
    //find the statue in items_statues array
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
    //find the statue in items_statues array
    unsigned char *pos=memchr(items_statues,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_statues)||(pos>=items_statues+sizeof(items_statues)))
      pos=(unsigned char *)items_statues+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a furniture
 */
short is_furniture(const unsigned char *thing)
{
    //All furniture are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_furniture_stype(get_thing_subtype(thing));
}

/*
 * Returns if the thing subtype represents a furniture
 */
short is_furniture_stype(const unsigned char stype_idx)
{
     int array_count=sizeof(items_furniture)/sizeof(unsigned char);
    //these are listed in items_furniture array
    char *pos=memchr(items_furniture,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
}

/*
 * Returns subtype of a next furniture
 */
unsigned char get_furniture_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_furniture)/sizeof(unsigned char);
    //find the furniture in items_furniture array
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
    //find the furniture in items_furniture array
    unsigned char *pos=memchr(items_furniture,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_furniture)||(pos>=items_furniture+sizeof(items_furniture)))
      pos=(unsigned char *)items_furniture+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

/*
 * Returns if the thing is a food
 */
short is_food(const unsigned char *thing)
{
    //All food are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_food_stype(get_thing_subtype(thing));
}

/*
 * Returns if the subtype is a food
 */
short is_food_stype(const unsigned char stype_idx)
{
     int array_count=sizeof(items_food)/sizeof(unsigned char);
    //these are listed in items_food array
    char *pos=memchr(items_food,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
}

/*
 * Returns subtype of a next food
 */
unsigned char get_food_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_food)/sizeof(unsigned char);
    //find the food in items_food array
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
    //find the food in items_food array
    unsigned char *pos=memchr(items_food,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof(unsigned char);
    if ((pos<items_food)||(pos>=items_food+sizeof(items_food)))
      pos=(unsigned char *)items_food+(array_count-1)*sizeof(unsigned char);
    return *pos;
}

short is_gold(const unsigned char *thing)
{
    //All are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_gold_stype(get_thing_subtype(thing));
}

short is_gold_stype(const unsigned char stype_idx)
{
     int array_count=sizeof(items_gold)/sizeof(unsigned char);
    //these they are listed in items_gold array
    char *pos=memchr(items_gold,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
}

/*
 * Returns subtype of a next gold
 */
unsigned char get_gold_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_gold)/sizeof(unsigned char);
    //find the gold in items_gold array
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
    //find the gold in items_gold array
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
    //All torches are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_torch_stype(get_thing_subtype(thing));
}

short is_torch_stype(const unsigned char stype_idx)
{
    int array_count=sizeof(items_torches)/sizeof(unsigned char);
    //all such objects must be listed in the correct array
    char *pos=memchr(items_torches,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
}

/*
 * Returns subtype of a next torch
 */
unsigned char get_torch_next(const unsigned char stype_idx)
{
     int array_count=sizeof(items_torches)/sizeof(unsigned char);
    //find the torch in items_torches array
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
    //find the torch in items_torches array
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
    //All heartflames are items
    if (get_thing_type(thing) != THING_TYPE_ITEM)
      return false;
    return is_heartflame_stype(get_thing_subtype(thing));
}

short is_heartflame_stype(const unsigned char stype_idx)
{
    int array_count=sizeof(items_heartflames)/sizeof(unsigned char);
    //all such objects must be listed in the correct array
    char *pos=memchr(items_heartflames,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
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
    //find the item in array
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
    int sizeof_itm=sizeof(unsigned char);
    int array_count=sizeof_arr/sizeof_itm;
    //find the item in array
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof_itm;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr+(array_count-1)*sizeof_itm;
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

short is_room_thing(const unsigned char *thing)
{
  switch (get_thing_type(thing))
  {
  case THING_TYPE_ROOMEFFECT:
      switch (get_thing_subtype(thing))
      {
      case ROOMEFC_SUBTP_ENTRICE:  // Entrance ice room effect
      case ROOMEFC_SUBTP_DRYICE:   //Graveyard dry ice
        return true;
      }
      return false;
  case THING_TYPE_DOOR:
      return true;
  case THING_TYPE_ITEM:
    { // Everything from ROOMEQUIP category is room_thing
      int cat_idx=get_item_category(get_thing_subtype(thing));
      if (cat_idx==ITEM_CATEGR_ROOMEQUIP)
        return true;
      // Also some other items which are not ROOMEQUIP
      switch (get_thing_subtype(thing))
      {
      case ITEM_SUBTYPE_TORCH:     //the torch on claimed wall
      case ITEM_SUBTYPE_SPINNKEY:  //Key marking closed doors
        return true;
      }
    };return false;
  default:
    return false;
  }
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
    int array_count=sizeof(items_litthings)/sizeof(unsigned char);
    //all such objects must be listed in the correct array
    char *pos=memchr(items_litthings,stype_idx,array_count);
    if (pos!=NULL) return true;
    return false;
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
    //find the item in array
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
    int sizeof_itm=sizeof(unsigned char);
    int array_count=sizeof_arr/sizeof_itm;
    //find the item in array
    unsigned char *pos=memchr(arr,stype_idx,array_count);
    if (pos!=NULL) pos-=sizeof_itm;
    if ((pos<arr)||(pos>=arr+sizeof_arr))
      pos=arr+(array_count-1)*sizeof_itm;
    return *pos;
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
    { // Everything from ROOMEQUIP category affects clm entries
      int cat_idx=get_item_category(get_thing_subtype(thing));
      if (cat_idx==ITEM_CATEGR_ROOMEQUIP)
        return true;
      // Also some other items which are not ROOMEQUIP
      switch (get_thing_subtype(thing))
      {
      case ITEM_SUBTYPE_TORCH:     //the torch on claimed wall
      case ITEM_SUBTYPE_TORCHUN:   //the torch on unclaimed dirt
        return true;
      }
    };return false;
  default:
    return false;
  }
}

/*
 * Returns if the object is crucial for the game. Such object
 * is not easily removed by the automatic placers.
 */
short is_crucial_thing(const unsigned char *thing)
{
  switch (get_thing_type(thing))
  {
  case THING_TYPE_ITEM:
      switch (get_thing_subtype(thing))
      {
      case ITEM_SUBTYPE_DNHEART:
      case ITEM_SUBTYPE_HEROGATE:
        return true;
      }
    return false;
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
          return "Gold (250)";
        case ITEM_SUBTYPE_TORCHUN:
          return "Unlit torch";
        case ITEM_SUBTYPE_STATUEWO:
          return "Statue wo";
        case ITEM_SUBTYPE_CHICKNGRW:
          return "Growing Chicken";
        case ITEM_SUBTYPE_CHICKN:
          return "Chicken";
        //Spells
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
        case ITEM_SUBTYPE_CHICKN2:
          return "Chicken";
        case ITEM_SUBTYPE_SPINNKEY:
          return "Spinning key";
        //Spells
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
        case ITEM_SUBTYPE_UNKN32: 
          return "unknown32";

        //UNTESTED - indices may differ by one
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

        //Creature lairs
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

        //Dungeon Specials
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
        //Traps
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

        //Workshop items
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
        //Room equipment
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

        //Special effects
        case ITEM_SUBTYPE_PWSIGHT:
          return "Power Sight";
        case ITEM_SUBTYPE_PWLIGHTNG:
          return "Power Lightning";
        //Other, mostly untested
        case ITEM_SUBTYPE_TORTURER:
          return "Torturer";
        case ITEM_SUBTYPE_LAIRORC:
          return "Orc Lair";
        case ITEM_SUBTYPE_PWHANDGLD:
          return "Power Hand Gold";
        case ITEM_SUBTYPE_COIN:
          return "Coin";

        case ITEM_SUBTYPE_STATUE2:
          return "Statue 2";
        case ITEM_SUBTYPE_STATUE3:
          return "Statue 3";
        case ITEM_SUBTYPE_STATUE4:
          return "Statue 4";
        case ITEM_SUBTYPE_STATUE5:
          return "Statue 5";
        case ITEM_SUBTYPE_STATUE6:
          return "Statue 6";
        case ITEM_SUBTYPE_SPELLARMG:
          return "Armageddon Spell";
        default:
          return "Unknown";
      }
}

/*
 * Returns object type name which is max 5 chars long
 */
char *get_item_subtype_shortname(unsigned short stype_idx)
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
        case ITEM_SUBTYPE_STATUE6:
          return "Statu";
        case ITEM_SUBTYPE_DNHEART:
          return "Heart";
        case ITEM_SUBTYPE_GOLD:
          return "Gold";
        case ITEM_SUBTYPE_CHICKNGRW:
        case ITEM_SUBTYPE_CHICKN:
        case ITEM_SUBTYPE_CHICKNSTB:
        case ITEM_SUBTYPE_CHICKNWOB:
        case ITEM_SUBTYPE_CHICKNCRK:
        case ITEM_SUBTYPE_CHICKN2:
          return "Food";
        //Spells
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
          return "Spangl";

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
          return "SpKey";

        case ITEM_SUBTYPE_HEROGATE:
          return "HGate";
        case ITEM_SUBTYPE_UNKN32: 
          return "unk32";

        case ITEM_SUBTYPE_ARMOUR: 
          return "Armor";
        case ITEM_SUBTYPE_GLDHOARD1: 
        case ITEM_SUBTYPE_GLDHOARD2: 
        case ITEM_SUBTYPE_GLDHOARD3: 
        case ITEM_SUBTYPE_GLDHOARD4: 
        case ITEM_SUBTYPE_GLDHOARD5: 
          return "GldHr";
        //Creature lairs
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
        //Dungeon Specials
        case ITEM_SUBTYPE_SPREVMAP: 
        case ITEM_SUBTYPE_SPRESURCT:
        case ITEM_SUBTYPE_SPTRANSFR:
        case ITEM_SUBTYPE_SPSTEALHR:
        case ITEM_SUBTYPE_SPMULTPLY:
        case ITEM_SUBTYPE_SPINCLEV:
        case ITEM_SUBTYPE_SPMKSAFE:
        case ITEM_SUBTYPE_SPHIDNWRL:
          return "SpcBx";
        //Traps
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
        //Workshop items
        case ITEM_SUBTYPE_DBWOOD:
        case ITEM_SUBTYPE_DBBRACE:
        case ITEM_SUBTYPE_DBSTEEL:
        case ITEM_SUBTYPE_DBMAGIC:
          return "DorBx";
        case ITEM_SUBTYPE_WBITEM:
          return "WshBx";
        //Room equipment
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
        case ITEM_SUBTYPE_COIN:
          return "Coin";
        default:
          return "thing";
      }
}

/*
 * Returns itam category for given subtype
 */
int get_item_category(unsigned char stype_idx)
{
      switch (stype_idx)
      {
        case ITEM_SUBTYPE_NULL:
        case ITEM_SUBTYPE_NULL1:
        case ITEM_SUBTYPE_NULL2:
          return ITEM_CATEGR_NULL;

        case ITEM_SUBTYPE_GOLDCHEST:
        case ITEM_SUBTYPE_GOLD:
        case ITEM_SUBTYPE_COIN:
        case ITEM_SUBTYPE_GLDHOARD1: 
        case ITEM_SUBTYPE_GLDHOARD2: 
        case ITEM_SUBTYPE_GLDHOARD3: 
        case ITEM_SUBTYPE_GLDHOARD4: 
        case ITEM_SUBTYPE_GLDHOARD5: 
          return ITEM_CATEGR_GOLD;

        case ITEM_SUBTYPE_STATUEWO:
        case ITEM_SUBTYPE_STATUHORN:
        case ITEM_SUBTYPE_STATUE2:
        case ITEM_SUBTYPE_STATUE3:
        case ITEM_SUBTYPE_STATUE4:
        case ITEM_SUBTYPE_STATUE5:
        case ITEM_SUBTYPE_STATUE6:
          return ITEM_CATEGR_STATUES;

        case ITEM_SUBTYPE_DNHEART:
        case ITEM_SUBTYPE_HEROGATE:
        case ITEM_SUBTYPE_TEMPLESTA:
        case ITEM_SUBTYPE_TEMPLESPN:
        case ITEM_SUBTYPE_ANVIL:
        case ITEM_SUBTYPE_PRISONBAR:
        case ITEM_SUBTYPE_SCAVNGEYE:
        case ITEM_SUBTYPE_WRKSHPMCH:
        case ITEM_SUBTYPE_GURDFLAGR:
        case ITEM_SUBTYPE_GURDFLAGB:
        case ITEM_SUBTYPE_GURDFLAGG:
        case ITEM_SUBTYPE_GURDFLAGY:
        case ITEM_SUBTYPE_FLAGPOST:
        case ITEM_SUBTYPE_CANDLSTCK:
        case ITEM_SUBTYPE_GRAVSTONE:
        case ITEM_SUBTYPE_TRAINPOST:
        case ITEM_SUBTYPE_TORTSPIKE:
        case ITEM_SUBTYPE_HEARTFLMR:
        case ITEM_SUBTYPE_HEARTFLMB:
        case ITEM_SUBTYPE_HEARTFLMG:
        case ITEM_SUBTYPE_HEARTFLMY:
        case ITEM_SUBTYPE_TORTURER:
          return ITEM_CATEGR_ROOMEQUIP;

        case ITEM_SUBTYPE_CHICKNGRW:
        case ITEM_SUBTYPE_CHICKN:
        case ITEM_SUBTYPE_CHICKNSTB:
        case ITEM_SUBTYPE_CHICKNWOB:
        case ITEM_SUBTYPE_CHICKNCRK:
        case ITEM_SUBTYPE_CHICKN2:
          return ITEM_CATEGR_FOOD;

        //Spells
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
          return ITEM_CATEGR_SPELLBOOK;

        case ITEM_SUBTYPE_TORCH:
        case ITEM_SUBTYPE_TORCHUN:
          return ITEM_CATEGR_LIGHTS;

        //Creature lairs
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
          return ITEM_CATEGR_CREATLAIR;

        //Dungeon Specials
        case ITEM_SUBTYPE_SPREVMAP: 
        case ITEM_SUBTYPE_SPRESURCT:
        case ITEM_SUBTYPE_SPTRANSFR:
        case ITEM_SUBTYPE_SPSTEALHR:
        case ITEM_SUBTYPE_SPMULTPLY:
        case ITEM_SUBTYPE_SPINCLEV:
        case ITEM_SUBTYPE_SPMKSAFE:
        case ITEM_SUBTYPE_SPHIDNWRL:
          return ITEM_CATEGR_SPECIALBOX;

        //Traps and other workshop items
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
        case ITEM_SUBTYPE_DBWOOD:
        case ITEM_SUBTYPE_DBBRACE:
        case ITEM_SUBTYPE_DBSTEEL:
        case ITEM_SUBTYPE_DBMAGIC:
        case ITEM_SUBTYPE_WBITEM:
          return ITEM_CATEGR_WRKSHOPBOX;

        case ITEM_SUBTYPE_PWHAND:
        case ITEM_SUBTYPE_PWHANDGRB:
        case ITEM_SUBTYPE_PWHANDWHP:
        case ITEM_SUBTYPE_PWHANDGLD:
        case ITEM_SUBTYPE_DISEASE:
        case ITEM_SUBTYPE_PWSIGHT:
        case ITEM_SUBTYPE_PWLIGHTNG:
          return ITEM_CATEGR_PWHNDEFFCT;

        case ITEM_SUBTYPE_POTION1:
        case ITEM_SUBTYPE_POTION2:
        case ITEM_SUBTYPE_POTION3:
        case ITEM_SUBTYPE_BARREL:
        case ITEM_SUBTYPE_SPINNKEY:
        case ITEM_SUBTYPE_ARMOUR: 
          return ITEM_CATEGR_VARIOUS;
        case ITEM_SUBTYPE_UNKN32: 
        default:
          return ITEM_CATEGR_UNKNOWN;
      }
}

/*
 * Returns creature type name string
 */
char *get_creature_subtype_fullname(unsigned short stype_idx)
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
char *get_creature_subtype_shortname(unsigned short stype_idx)
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
char *get_trap_subtype_fullname(unsigned short stype_idx)
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
char *get_door_subtype_fullname(unsigned short stype_idx)
{
     int types_count=sizeof(doors)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)doors[stype_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns item category text for given subtype
 */
char *get_item_category_fullname(unsigned short stype_idx)
{
     int cat_idx=get_item_category(stype_idx);
     switch (cat_idx)
     {
     case ITEM_CATEGR_NULL:
          return "Null objects";
     case ITEM_CATEGR_VARIOUS:
          return "Various objects";
     case ITEM_CATEGR_ROOMEQUIP:
          return "Room equipment";
     case ITEM_CATEGR_SPELLBOOK:
          return "Spell books";
     case ITEM_CATEGR_STATUES:
          return "Statues";
     case ITEM_CATEGR_GOLD:
          return "Valuable items";
     case ITEM_CATEGR_FOOD:
          return "Creature food";
     case ITEM_CATEGR_CREATLAIR:
          return "Creature lairs";
     case ITEM_CATEGR_SPECIALBOX:
          return "Dungeon Special box";
     case ITEM_CATEGR_WRKSHOPBOX:
          return "Workshop item box";
     case ITEM_CATEGR_PWHNDEFFCT:
          return "Power Hand effect";
     case ITEM_CATEGR_LIGHTS:
          return "Light objects";
     default:
          return "Unknown category";
     }
}

/*
 * Returns room effect description string
 */
char *get_roomeffect_subtype_fullname(unsigned char stype_idx)
{
     int types_count=sizeof(roomeffects)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)roomeffects[stype_idx];
     else
       return "unknown(?!)";
}

short item_verify(unsigned char *thing, char *err_msg)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  switch (get_item_category(stype_idx))
  {
    case ITEM_CATEGR_VARIOUS:
    case ITEM_CATEGR_ROOMEQUIP:
    case ITEM_CATEGR_STATUES:
    case ITEM_CATEGR_SPELLBOOK:
    case ITEM_CATEGR_GOLD:
    case ITEM_CATEGR_FOOD:
    case ITEM_CATEGR_CREATLAIR:
    case ITEM_CATEGR_SPECIALBOX:
    case ITEM_CATEGR_WRKSHOPBOX:
    case ITEM_CATEGR_PWHNDEFFCT:
    case ITEM_CATEGR_LIGHTS:
      break;
    case ITEM_CATEGR_NULL:
      sprintf(err_msg,"Null item subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
    default:
      sprintf(err_msg,"Unknown item subtype (%d)",(int)stype_idx);
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
      // Generate warning if sensitile coordinates are wrong.
      // Ignore such error for items which usually have it wrongly set.
      int sen_tlx,sen_tly;
      sen_tlx=sen_tl%MAP_SIZE_X;
      sen_tly=sen_tl/MAP_SIZE_Y;
      if (((tx<sen_tlx-1)||(tx>sen_tlx+1)||
           (ty<sen_tly-1)||(ty>sen_tly+1)) &&
           (stype_idx!=ITEM_SUBTYPE_SPINNKEY))
      {
          sprintf(err_msg,"Item has wrong sensitive tile number");
          return VERIF_WARN;
      }
  }
  return VERIF_OK;
}

short creature_verify(unsigned char *thing, char *err_msg)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  if ((stype_idx>CREATR_SUBTP_FLOAT)||(stype_idx<CREATR_SUBTP_WIZRD))
  {
      sprintf(err_msg,"Unknown creature subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
  }
  return VERIF_OK;
}

short roomeffect_verify(unsigned char *thing, char *err_msg)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  if ((stype_idx>ROOMEFC_SUBTP_DRYICE)||(stype_idx<ROOMEFC_SUBTP_LAVA))
  {
      sprintf(err_msg,"Unknown room effect subtype (%d)",(int)stype_idx);
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
      // Generate warning if sensitile coordinates are wrong.
      // Ignore such error for effects which usually have it wrongly set.
      int sen_tlx,sen_tly;
      sen_tlx=sen_tl%MAP_SIZE_X;
      sen_tly=sen_tl/MAP_SIZE_X;
      if (((tx<sen_tlx-1)||(tx>sen_tlx+1)||
           (ty<sen_tly-1)||(ty>sen_tly+1)) &&
           (stype_idx!=ROOMEFC_SUBTP_ENTRICE))
      {
          sprintf(err_msg,"Room effect has wrong sensitive tile number");
          return VERIF_WARN;
      }
  }
  return VERIF_OK;
}

short door_verify(unsigned char *thing, char *err_msg)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  if ((stype_idx>DOOR_SUBTYPE_MAGIC)||(stype_idx<DOOR_SUBTYPE_WOOD))
  {
      sprintf(err_msg,"Unknown door subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
  }
  return VERIF_OK;
}

short trap_verify(unsigned char *thing, char *err_msg)
{
  unsigned char stype_idx=get_thing_subtype(thing);
  if ((stype_idx>TRAP_SUBTYPE_LAVA)||(stype_idx<TRAP_SUBTYPE_BOULDER))
  {
      sprintf(err_msg,"Unknown trap subtype (%d)",(int)stype_idx);
      return VERIF_WARN;
  }
  return VERIF_OK;
}

short thing_verify(unsigned char *thing, char *err_msg)
{
  short result;
  int pos_h=get_thing_subtile_h(thing);
  int pos_x=get_thing_subtile_x(thing);
  int pos_y=get_thing_subtile_y(thing);
  if ((pos_h>=MAP_SUBNUM_H)||(pos_x>=MAP_SUBTOTAL_X)||(pos_y>=MAP_SUBTOTAL_Y))
  {
    sprintf(err_msg,"Thing position (%d,%d,%d) invalid",pos_x,pos_y,pos_h);
    return VERIF_WARN;
  }
  switch(get_thing_type(thing))
  {
    case THING_TYPE_ITEM:
      return item_verify(thing, err_msg);
    case THING_TYPE_CREATURE:
      return creature_verify(thing, err_msg);
    case THING_TYPE_ROOMEFFECT:
      return roomeffect_verify(thing, err_msg);
    case THING_TYPE_TRAP:
      return trap_verify(thing, err_msg);
    case THING_TYPE_DOOR:
      return door_verify(thing, err_msg);
    default:
      sprintf(err_msg,"Unknown thing type (%d)",(int)get_thing_type(thing));
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
